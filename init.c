#include "configs.h"
#include <sys/stat.h>

int init(){
    // Check if .nit already exists
    struct stat st = {0};
    if(stat(".nit", &st) == 0) {
        printf("This is already a nit repo.");
        return 1;
    }
    if(mkdir(".nit", 0744 )==0){
        printf("nit repo initialized.\n");
    }  
    
        // If exists, exit saying already a nit repo
        // Else create .nit, and inside HEAD, refs/, info/ etc
    
}