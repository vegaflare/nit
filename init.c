#include "configs.h"
#include "common-funcs.h"

int cmd_init()
{
    // Check if .nit already exists
    struct stat st = {0};
    if (stat(".nit", &st) == 0)
    {
        // Just exit if there's already a .nit. No reinit
        // at this point.
        printf("This is already a nit repo.\n");
        return 1;
    }
    if (make_dir(".nit") >= 0)
    {
        FILE *head, *description;
        head = fopen(".nit/HEAD", "w");
        fprintf(head, "%s%s\n", head_prefix, default_branch);
        fclose(head);

        // Create a description file
        description = fopen(".nit/description", "w");
        fprintf(description, "Unnamed repo. Edit this file to name the repo.\n");
        fclose(description);

        if (make_dir(".nit/refs") < 0 || make_dir(".nit/refs/heads") < 0 || make_dir(".nit/objects") < 0)
        {
            printf("Error initializing repo.\n");
            return 2;
        }
        printf("nit repo initialized.\n");
    }

    // If exists, exit saying already a nit repo
    // Else create .nit, and inside HEAD, refs/, info/ etc
}