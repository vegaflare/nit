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
#include <stdlib.h>

#define DEFAULT_DIR_C_MODE 0755

const char head_prefix[] = "refs: refs/heads/";
const char default_branch[] = "master";

int cmd_init();
int cmd_status(const char *branch);
