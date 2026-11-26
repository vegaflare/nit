#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "nit-idx.h"
#include "common-funcs.h"
/*
    type    -> type of file
    mtime   -> last modified time
    mode    -> new, updated, deleted
*/

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


int read_idx_header(struct nit_idx_header *header, FILE *idx)
{

   // FILE *idx = fopen(".nit/index", "rb");
    if (!idx)
    {
        perror("Error opening index.\n");
    }
    unsigned char buf[12]; // 12 byte buffer to read the header
    if (fseek(idx, 0, SEEK_SET) != 0 || fread(buf, 1, sizeof buf, idx) != sizeof buf)
    {
        perror("Error reading index file");
        printf("errorno %d\n", errno);
        return -1;
    }
    memcpy(header->signature, buf, 4);
    header->version = get_be32(buf + 4);
    header->n_entries = get_be32(buf + 8);
    if (memcmp(header->signature, "NITC", 4) != 0)
        return -2;
    return 0;
}

int write_idx_header(struct nit_idx_header *h, FILE *idx)
{

    //FILE *idx = fopen(".nit/index", "r+b");
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
    fflush(idx);
    return 0;
}

//  For updating the header with new version and count
//  when new entries are added.
int update_idx_header(uint8_t flag, int update, FILE *idx)
{
    struct nit_idx_header h = {0};
    if (read_idx_header(&h, idx) != 0)
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
    if (write_idx_header(&h, idx) != 0)
    {
        return 4;
    }
    return 0;
}

int write_idx_entry(FILE *f, struct nit_idx_entry *e)
{
    if (e->len > UINT32_MAX)
        return -1;
    uint32_t type = htonl(e->type);
    uint32_t mtime = htonl(e->mtime);
    uint32_t len = htonl(e->len);
  //  printf("1: %d, 2: %d, %s", len, e->len, e->filename);
    memcpy(e->mode, "NW", 2);  // NW for new entry
    if (fseek(f,0, SEEK_END) != 0|| 
        fwrite(&type, sizeof type, 1, f) != 1 ||
        fwrite(e->mode, 1, 2, f) != 2||
        fwrite(&mtime, sizeof mtime, 1, f) != 1 ||
        fwrite(&len, sizeof len, 1, f) != 1 ||
        fwrite(e->filename, 1, e->len, f) != e->len)
    {
        perror("Error writing file");
        return -1;
    }
    fflush(f);
    if (update_idx_header(IDX_INCR, 0, f) != 0)
    {
        return 2;
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
        FILE *idx = fopen(".nit/index", "w+b");
        if (write_idx_header(&h, idx) != 0)
        {
            return 4;
        }
        printf("New index created.\n");
        fclose(idx);
        return true;
    }
}

int read_idx_entry(struct nit_idx_entry *e, int index, FILE *f)
{
    //FILE *f = fopen(".nit/index", "rb");
    if (f == NULL)
    {
        perror("Failed reading index");
        return 2;
    }
    if(ftell(f) < 12){
        if(fseek(f, 12, SEEK_SET) != 0 || feof(f)){
            perror("Reading index entry past header failed");
            return 2;
        }
    }
    unsigned char buf[14];

    if(fread(buf, 1, sizeof buf, f) != sizeof buf){
        
        if(feof(f)){
            printf("Reached file end.\n");
            return 1;
        }

        perror("Error reading entry");
        return 2;
    }
    e->type = get_be32(buf);
    memcpy(e->mode,buf +4, 2);
    e->mtime = get_be32(buf +6);
    e->len = get_be32(buf +10);
    e->filename = malloc(e->len+1);
    if(!e->filename){
        perror("Invalid index entry");
        return 3;
    }
     if(fread(e->filename, 1, e->len, f) != e->len){
        perror("Error reading entry");
        return 2;
    }
    // Add null
    e->filename[e->len] = '\0';
    
    if(feof(f)){
        return 1;
    }
    return 0;
}

//  Check if a file is already added into the index

int traverse_index(const char **argv, int argc)
{
    struct nit_idx_header h;
    FILE *idx = fopen(".nit/index", "r+b");
    if(!idx){
        return -1;
    }
    read_idx_header(&h, idx);
    printf("%.4s\tv%d\t%d\n", h.signature, h.version, h.n_entries);
    struct nit_idx_entry e;
    char *sha1sum = malloc(41); // size of sha1 sum + null byte
   for(int i = 0; i < h.n_entries; ++i){
        read_idx_entry(&e, 0, idx);
        // hash calculation is done here to verify the working of the function
        // this will be removed from here and ported to index update and index
        // will have a hash of the file in it
        if(hash_file(e.filename, sha1sum)){
        return -1;
        }
        printf("%d\t%.2s\t%d\t%d\t%s\t%s", e.type, e.mode, e.mtime, e.len, e.filename,sha1sum);
  
    }    
    fclose(idx);
    free(sha1sum);
    free(e.filename);

}

// Recursive fn for searching a file name
bool  index_lookup(FILE *idx, struct nit_idx_entry *e, const char *fname, uint32_t count)
{
    if(ftell(idx) < 12){
        if(fseek(idx, 12, SEEK_SET) != 0 || feof(idx)){
            perror("Reading index entry past header failed");
            return false;
        }
    }
   for(int i = 0; i < count; ++i){
        if(read_idx_entry(e, 0, idx) == 0){
        if(strcmp(e->filename, fname)==0){
            return true;
        }
        else if(feof(idx)){
            return false;
        }
        
        return index_lookup(idx, e,fname, count--);
    }
    return false;
    }    


}
    


