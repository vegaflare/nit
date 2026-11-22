/*  All .c files are to include this configs.h as a 
    central include file. This will help avoid
    adding same headers over and over again across
    multiple c files.
*/

#ifndef CONFIG_H
#define CONFIG_H
#endif

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

#define DEFAULT_DIR_C_MODE 0755

int init();
int make_dir(const char *path);