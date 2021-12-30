#include "crane.h"
#include "docks.h"
#include "container.h"
#include <stdio.h>

void pickAndPlace(char trs1, int x1, char trs2, int x2, Docks* shmem){
    // /!\ this function assumes the docks sémaphore is allready locked

    printShmem(getShmid());
    container buffer;
    switch(trs1){
        case 'T':
            buffer = shmem->trucksSharedDock.cont[x1];
            shmem->trucksSharedDock.cont[x1].id = -1;
            shmem->trucksSharedDock.cont[x1].dest = 0;
            break;
        case 't':
            buffer = shmem->trainSharedDock.cont[x1];
            shmem->trainSharedDock.cont[x1].id = -1;
            shmem->trainSharedDock.cont[x1].dest = 0;
            break;
        default: //boats
            buffer = shmem->boatSharedDock.cont[x2];
            shmem->boatSharedDock.cont[x1].id = -1;
            shmem->boatSharedDock.cont[x1].dest = 0;
            break;
    }
    printf("\n[CRANE]Got the container %d with destination %c\n",buffer.id, buffer.dest);

    switch(trs2){
        case 'T':
            shmem->trucksSharedDock.cont[x2] = buffer;
            break;
        case 't':
            shmem->trucksSharedDock.cont[x2] = buffer;
            break;
        default: //boats
            shmem->trucksSharedDock.cont[x2] = buffer;
            break;
    }
    printShmem(getShmid());
}

int findMatch(char dest, char* trs, int* index, Docks* docks){
    for(int i=0; i<10; i++){
        if(docks->trucksSharedDock.cont[i].id != -1 && docks->trucksSharedDock.cont[i].dest == dest && docks->trucksSharedDock.trs[i].dest != dest){
            *trs = 'T';
            *index = i;
            return 0;
        }
    }
    return -1; 
}




void crane(Crane* c){
    Docks* docks = (Docks *)shmat(c->shmid, NULL, 0);

    while(1){

    }


}

