#include "configs.h"
#include <string.h>
#include "nit-idx.h"

#define HEADS_PATH ".nit/refs/heads"
#define HEADS_PATH_LEN 15

int cmd_status(const char **argv, int argc)
{
    // Check and display the stauts of current repo.
    // TODO: add a func to get the current branch name
    // Current branch is available in HEAD, needs to
    // be read using a fuc that processes that string.

    // Hard-coding branch to master for now
    const char *branch = NULL;
    if (branch == NULL)
    {
        branch = default_branch;
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

/*  This is a piping method used interally (accessible
    via cli as well) for hashing a file, commit message,
    trees etc. This is needed for further progressing my
    implementation.
*/
int hash_object_cmd(char **argv, int argc)
{
    //  args can be (for now) just --stdin
    if (argc > 1)
    {
        printf(nit_help);
        return -1;
    }
    for (int i = 1; i < argc; ++i)
    {
    }
}

/*  Update index is used to add files to a staging area
    We will be using different file modes from git
    10001 : Regular file
    10002 : tree
*/
int cmd_update_index(const char **argv, int argc)
{
    if (argc != 3)
    {
        printf("Invalid function call: update_index_cmd\n");
        return -1;
    }
    if (!initialize_index())
    {
        return -5;
    }
    struct stat st = {0};
    if (stat(argv[2], &st) != 0)
    {
        printf("file '%s' doesn't exist.\n", argv[2]);
        return 2;
    }
    struct nit_idx_entry ie;
    ie.type = atoi(argv[1]);
    ie.mtime = st.st_mtime;
    ie.filename = argv[2];
    FILE *index = fopen(".nit/index", "ab");
    // fprintf(index, "%s\t%d\t%s\n", argv[1], st.st_mtime, argv[2]);
    if (write_idx_entry(index, &ie) == 0)
    {
        printf("Index updated:\n\t%s    %d    %s\n", argv[1], st.st_mtime, argv[2]);
        fclose(index);
        return 0;
    }
}