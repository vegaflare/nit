#ifndef NIT_IDX_H
#define NIT_IDX_H

/*  Index update flags
    IDX_INCR increments the count by 1
    IDX_UPDT increments the count by a specific number
*/
#define IDX_INCR 1
#define IDX_UPDT 2



#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>

static uint32_t get_be32(const unsigned char *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
    return ntohl(v);
}

static void put_be32(unsigned char *p, uint32_t v)
{
    v = htonl(v);
    memcpy(p, &v, 4);
}

struct nit_idx_entry
{
    uint16_t type;
    uint32_t mtime;
    const char *filename;
};

struct nit_idx_header
{
    uint32_t version; // Will not be used for now
    uint32_t n_entries;
    char signature[4];
};

int read_idx_header(struct nit_idx_header *header)
{

    FILE *idx = fopen(".nit/index", "rb");
    if (!idx)
    {
        perror("Error opening index.\n");
    }
    unsigned char buf[12]; // 12 byte buffer to read the header
    if (fread(buf, 1, sizeof buf, idx) != sizeof buf)
    {
        perror("Error reading index file");
        return -1;
    }
    memcpy(header->signature, buf, 4);
    header->version = get_be32(buf + 4);
    header->n_entries = get_be32(buf + 8);
    if (memcmp(header->signature, "NITC", 4) != 0)
        return -2;
    return 0;
}

static int write_idx_header(struct nit_idx_header *h)
{

    FILE *idx = fopen(".nit/index", "r+b");
    if (!idx)
    {
        perror("Error opening index.\n");
    }
    unsigned char buf[12];
    memcpy(buf, h->signature, 4);
    put_be32(buf + 4, h->version);
    put_be32(buf + 8, h->n_entries);

    if (fseek(idx, 0, SEEK_SET) != 0 ||
        fwrite(buf, 1, sizeof buf, idx) != sizeof buf)
    {
        perror("Error writing header\n");
        return -1;
    }
    return 0;
}

//  For updating the header with new version and count
//  when new entries are added.
int update_idx_header(uint8_t flag, int update)
{
    struct nit_idx_header h;
    if (read_idx_header(&h) != 0)
    {
        return 5;
    }
    printf("Header:\n\t%s(size: %d)\tv%d\t%d\n", h.signature, strlen(h.signature), h.version, h.n_entries);
    if (flag == 1)
    {
        h.n_entries++;
    }
    else
    {
        h.n_entries += update;
    }
    if (write_idx_header(&h) != 0)
    {
        return 4;
    }
    return 0;
}


static int write_idx_entry(FILE *f, const struct nit_idx_entry *e)
{
    size_t len = strlen(e->filename);
    if (len > UINT16_MAX)
        return -1;
    uint16_t type = htons(e->type);
    uint32_t mtime = htons(e->mtime);
    uint16_t nlen = htons((uint16_t)len);
    if(update_idx_header(IDX_INCR, 0) != 0){
        return 2;
    }
    if (fwrite(&type, sizeof type, 1, f) != 1 ||
        fwrite(&mtime, sizeof mtime, 1, f) != 1 ||
        fwrite(&nlen, sizeof nlen, 1, f) != 1 ||
        fwrite(e->filename, 1, len, f) != len)
    {
        printf("Error writing file\n");
        return -1;
    }
    return 0;
}



bool initialize_index()
{
    struct stat st = {0};

    if (stat(".nit/index", &st) == 0)
    {
        if (S_ISREG(st.st_mode))
        {
            printf("Index exists.\n");
            return true;
        }
        else
        {
            printf("Invalid index object already exists.\n");
            return false;
        }
    }
    else
    {
        // write default header
        struct nit_idx_header h;
        memcpy(h.signature, "NITC", 4); // Nit cache
        h.version = 0;
        h.n_entries = 0;
        FILE *idx = fopen(".nit/index", "wb");
        fclose(idx);
        if (write_idx_header(&h) != 0)
        {
            return 4;
        }
        printf("New index created.\n");
        return true;
    }
}




#endif
