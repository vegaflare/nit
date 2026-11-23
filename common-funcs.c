#include "configs.h"

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