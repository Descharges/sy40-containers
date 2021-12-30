#include "crane.h"
#include "docks.h"
#include "container.h"
#include <stdio.h>

void pickAndPlace(char trs1, int x1, char trs2, char int2, Docks* shmem){
    // /!\ this function assumes the docks sémaphore is allready locked

    container buffer;
    switch(trs1){
        case 'T':
            buffer = shmem->trucksSharedDock.cont[x1];
            shmem->trucksSharedDock.cont[x1].id = -1;
            shmem->trucksSharedDock.cont[x1].id = 0;
            break;
        case 't':
            buffer = shmem->trainSharedDock.cont[x1];
            shmem->trainSharedDock.cont[x1].id = -1;
            shmem->trainSharedDock.cont[x1].id = 0;
            break;
        default: //boats
            buffer = shmem->trucksSharedDock.cont[x1];
            shmem->trucksSharedDock.cont[x1].id = -1;
            shmem->trucksSharedDock.cont[x1].id = 0;
            break;
    }
    
    printf("[CRANE]Got the container %d with destination %c",buffer.id, buffer.dest);

}

