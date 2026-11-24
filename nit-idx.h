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



/*
    type    -> type of file
    mtime   -> last modified time
    mode    -> new, updated, deleted
*/
struct nit_idx_entry
{
    uint32_t type;
    char mode[2];
    uint32_t mtime;
    uint32_t len;
    char *filename;
};

struct nit_idx_header
{
    char signature[4];
    uint32_t version; // Will not be used for now
    uint32_t n_entries;
    
};

int read_idx_header(struct nit_idx_header *header, FILE *idx);
int write_idx_header(struct nit_idx_header *h, FILE *idx);

int update_idx_header(uint8_t flag, int update, FILE *idx);
int write_idx_entry(FILE *f, struct nit_idx_entry *e);
bool initialize_index(void);
int read_idx_entry(struct nit_idx_entry *e, int index, FILE *f);

int traverse_index(const char **argv, int argc);
bool  index_lookup(FILE *idx, struct nit_idx_entry *e, const char *fname, uint32_t count);

#endif
