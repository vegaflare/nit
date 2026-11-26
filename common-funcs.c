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

int hash_file(char *fname, char *output)
{
    FILE *f = fopen(fname, "rb");
    if (!f)
    {
        printf("Failed opening file: %s", fname);
        perror("fopen-hash_file");
        return -1;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);

    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0)
    {
        EVP_DigestUpdate(ctx, buf, n);
    }
    fclose(f);

    unsigned char hash[20]; // sha1 hash is 160 bits -> 20 bytes
    if (!output)
    {
        printf("Null pointer passed - hash_file\n");
        return -1;
    }
    unsigned int len = 0;

    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);
    for (int i = 0; i < 20; i++)
    {
        sprintf(output + i * 2, "%02x", hash[i]);
    }
    output[40] = '\0';
    return 0;
}