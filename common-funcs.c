#include "configs.h"

int make_dir(const char *path)
{
    if (mkdir(path, DEFAULT_DIR_C_MODE) == -1 && errno != EEXIST)
    {
        perror("path");
        return -1;
    }
    return 0;
}