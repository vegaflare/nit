/*  All .c files are to include this configs.h as a
    central include file. This will help avoid
    adding same headers over and over again across
    multiple c files.
*/

#ifndef CONFIG_H
#define CONFIG_H
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

#define DEFAULT_DIR_C_MODE 0755

extern const char head_prefix[];
extern const char default_branch[];
extern const char nit_help[];

int cmd_init();
int cmd_status(const char **argv, int argc);
int cmd_update_index(const char **argv, int argc);
