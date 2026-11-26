#include "configs.h"
#include <openssl/evp.h>

const char head_prefix[] = "refs: refs/heads/";
const char default_branch[] = "master";
const char nit_help[] = "Nit help: to be added.\n";
int make_dir(const char *path)
{
    if (mkdir(path, DEFAULT_DIR_C_MODE) == -1 && errno != EEXIST)
    {
        perror("path");
        return -1;
    }
    return 0;
}

bool hash_file(char *fname, unsigned char *hash){
    FILE *f =  fopen(fname, "rb");
    if(!f){
        perror("fopen-hash_file");
        return false;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);

    unsigned char buf[4096];
    size_t n;
    while((n = fread(buf, 1, sizeof buf, f)) > 0){
        EVP_DigestUpdate(ctx, buf, n);
    }
    fclose(f);

    // unsigned char *hash = malloc(20); // sha1 hash is 160 bits -> 20 bytes
    unsigned int len = 0;

    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);

    return true;
}