// Nit! my own version of git

#include "configs.h"
#include <string.h>

int main(int argc, const char **argv)
{
    if (argc <= 1)
    {
        printf("redirect to help.\n");
    }
    else
    {
        // Remove current program name from the args list
        argv++;
        argc--;
    }
    // Parse args
    for (int i = 0; i < argc; ++i)
    {
        // printf("Index %d: %s\n", i, argv[i]);
        if (strcmp(argv[0], "init") == 0)
        {
            printf("Init will be processed.\n");
            return cmd_init();
        }
        else if (strcmp(argv[0], "status") == 0)
        {
            return cmd_status(argv, argc);
        }
        else if (strcmp(argv[0], "update-index") == 0)
        {
            return cmd_update_index(argv, argc);
        }
    }
}