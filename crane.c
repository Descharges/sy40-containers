#include "crane.h"
#include "docks.h"
#include "container.h"
#include <stdio.h>
#include <signal.h>

void pickAndPlace(char trs1, int x1, char trs2, int x2, Docks* shmem){
    // /!\ this function assumes the docks sémaphore is allready locked

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
            buffer = shmem->boatSharedDock.cont[x1];
            shmem->boatSharedDock.cont[x1].id = -1;
            shmem->boatSharedDock.cont[x1].dest = 0;
            break;
    }

    switch(trs2){
        case 'T':
            shmem->trucksSharedDock.cont[x2] = buffer;
            break;
        case 't':
            shmem->trainSharedDock.cont[x2] = buffer;
            break;
        default: //boats
            shmem->boatSharedDock.cont[x2] = buffer;
            break;
    }
}

int findMatch(char dest, char* trs, int* index, Docks* docks){
    for(int i=0; i<10; i++){
        if(docks->trucksSharedDock.cont[i].id != -1 && docks->trucksSharedDock.cont[i].dest == dest && docks->trucksSharedDock.trs[i].dest != dest){
            *trs = 'T';
            *index = i;
            return 0;
        }
    }
    for(int i=0; i<10; i++){
        if(docks->trainSharedDock.cont[i].id != -1 && docks->trainSharedDock.cont[i].dest == dest && docks->trainSharedDock.trs[i/5].dest != dest){
            *trs = 't';
            *index = i;
            return 0;
        }
    }
    for(int i=0; i<6; i++){
        if(docks->boatSharedDock.cont[i].id != -1 && docks->boatSharedDock.cont[i].dest == dest && docks->boatSharedDock.trs[i/3].dest != dest){
            *trs = 'b';
            *index = i;
            return 0;
        }
    }
    return -1; 
}

int checkTransport(char trs, int index, Docks* docks){
    switch(trs){
        case 't':
        for(int i=0; i<5; i++){
            if(docks->trainSharedDock.trs[index].id == -1 || docks->trainSharedDock.trs[index].dest != docks->trainSharedDock.cont[(index*5)+i].dest){
                return -1;
            }
        }
        break;
        case 'b':
        for(int i=0; i<3; i++){
            if(docks->boatSharedDock.trs[index].id == -1 ||docks->boatSharedDock.trs[index].dest != docks->boatSharedDock.cont[(index*3)+i].dest){
                return -1;
            }
        }
        break;
        case 'T':
        if(docks->trucksSharedDock.trs[index].id == -1 || docks->trucksSharedDock.cont[index].dest != docks->trucksSharedDock.trs[index].dest){
            return -1;
        }

        break;
        
            
    }
    return 0;
}




void* craneFunc(Crane* c){
    Docks* docks = (Docks *)shmat(c->shmid, NULL, 0);
    int index = -1;
    char trs = 0;
    char dest = 0;

    while(1){
        sleep(5);

        lock(FULL);

        printShmem(getShmid());

        for (int i=0; i<10; i++) {
            if(docks->trucksSharedDock.cont[i].id == -1 && findMatch(docks->trucksSharedDock.trs[i].dest,&trs,&index,docks)==0){
                pickAndPlace(trs, index, 'T', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 'T');
            }
        }
        for (int i=0; i<10; i++) {
            if(docks->trainSharedDock.cont[i].id == -1 && findMatch(docks->trainSharedDock.trs[i/5].dest,&trs,&index,docks)==0){
                pickAndPlace(trs, index, 't', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 't');
            }
        }
        for (int i=0; i<6; i++) {
            if(docks->boatSharedDock.cont[i].id == -1 && findMatch(docks->boatSharedDock.trs[i/3].dest,&trs,&index,docks)==0){
                pickAndPlace(trs, index, 'b', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 'b');
            }
        }

        printShmem(getShmid());

        if(checkTransport('T', 9, docks)==0){
            pthread_kill(docks->trucksSharedDock.trs[9].tid, SIGUSR1);
        }

        if(checkTransport('t', 1, docks)==0){
            pthread_kill(docks->trainSharedDock.trs[1].tid, SIGUSR1);
        }

        if(checkTransport('b', 0, docks)==0){
            pthread_kill(docks->boatSharedDock.trs[0].tid, SIGUSR1);
        }

        if(checkTransport('b', 1, docks)==0){
            pthread_kill(docks->boatSharedDock.trs[1].tid, SIGUSR1);
        }



        unlock(FULL);
        

    }


}

