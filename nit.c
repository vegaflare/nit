#include "configs.h"

#define HEADS_PATH ".nit/refs/heads"
#define HEADS_PATH_LEN 15

int cmd_status(char **argv)
{
    // Check and display the stauts of current repo.
    // TODO: add a func to get the current branch name
    // Current branch is available in HEAD, needs to
    // be read using a fuc that processes that string.

    // Hard-coding branch to master for now
    char branch = "master";
    if (branch == NULL)
    {
        branch = &default_branch;
    }
    int result = 0;
    struct stat nit = {0};
    size_t len = strlen(branch);
    char *head = malloc(HEADS_PATH_LEN + len + 1);
    memcpy(head, HEADS_PATH, HEADS_PATH_LEN);
    memcpy(head + HEADS_PATH_LEN + 1, branch, len);

    if (stat(HEADS_PATH, &nit) == 0 && S_ISDIR(nit.st_mode))
    {
        if (stat(head, &nit) == 0 && S_ISREG(nit.st_mode))
        {
            printf("Is initialized, status reading not implemented.\n");
        }
        else
        {
            printf("Current branch '%s' is upto date", branch);
            result = 4;
        }
    }
    else
    {
        printf("Fatal: not a nit repo. Run 'nit init' to initialize a repo.\n");
        result = 3;
    }
    free(head);
    return result;
}