#include "crane.h"
#include "docks.h"
#include "container.h"
#include <pthread.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>

pthread_cond_t nextCrane = PTHREAD_COND_INITIALIZER;
pthread_mutex_t nextCraneMut = PTHREAD_MUTEX_INITIALIZER;

extern int end;
int cleaned = 0;

void sigintHandler(int signo){
    end = 1;
    printf("Fin enclenchée");
    fflush(stdout);
}


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

int findMatch(char dest, char* trs, int* index, Docks* docks, Crane* c){
    for(int i=c->id*3; i<(c->id*3)+3; i++){
        if(docks->boatSharedDock.cont[i].id != -1 && docks->boatSharedDock.cont[i].dest == dest && docks->boatSharedDock.trs[i/3].dest != dest){
            *trs = 'b';
            *index = i;
            return 0;
        }
    }
    for(int i=c->id*5; i<(c->id*5)+5; i++){
        if(docks->trainSharedDock.cont[i].id != -1 && docks->trainSharedDock.cont[i].dest == dest && docks->trainSharedDock.trs[i/5].dest != dest){
            *trs = 't';
            *index = i;
            return 0;
        }
    }
    for(int i=c->id*5; i<(c->id*5)+5; i++){
        if(docks->trucksSharedDock.cont[i].id != -1 && docks->trucksSharedDock.cont[i].dest == dest && docks->trucksSharedDock.trs[i].dest != dest){
            *trs = 'T';
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
    int vehicleGone = 0; 
    trsDest infoMsg; infoMsg.type = 1;
    int printCounter;

    struct sigaction unpauseSigaction;
    unpauseSigaction.sa_handler = sigintHandler;
    sigemptyset (&unpauseSigaction.sa_mask);
    unpauseSigaction.sa_flags = 0;
    sigaction(SIGINT, &unpauseSigaction, NULL);


    if(c->id != 1){
        pthread_mutex_lock(&nextCraneMut);
        pthread_cond_wait(&nextCrane, &nextCraneMut);
        pthread_mutex_unlock(&nextCraneMut);
    }

    while(end == 0){
        sleep(1);

        lock(FULL);

            printf("\n\n[CRANE ID=%d]Starting to operate\n",c->id);
            fflush(stdout);

        for (int i=(c->id *5); i>(c->id *5)+5; i++) {
            if(docks->trainSharedDock.cont[i].id == -1 && docks->trainSharedDock.trs[i/5].id != -1 && findMatch(docks->trainSharedDock.trs[i/5].dest,&trs,&index,docks,c)==0){
                pickAndPlace(trs, index, 't', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 't');
                fflush(stdout); 
            }
        }


        for (int i=c->id*3; i<(c->id*3)+3; i++) {
            if(docks->boatSharedDock.cont[i].id == -1 && docks->boatSharedDock.trs[i/3].id != -1 && findMatch(docks->boatSharedDock.trs[i/3].dest,&trs,&index,docks,c)==0){
                pickAndPlace(trs, index, 'b', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 'b');
            }
        }

        

        for (int i=c->id*5; i<(c->id*5)+5; i++) {
            if(docks->trucksSharedDock.cont[i].id == -1 && docks->trucksSharedDock.trs[i].id != -1 && findMatch(docks->trucksSharedDock.trs[i].dest,&trs,&index,docks,c)==0){
                pickAndPlace(trs, index, 'T', i, docks);
                printf("[CRANE] Took container [i=%d,trs=%c] to [i=%d, trs=%c]\n",index,trs, i, 'T');
            }
        }
        
        
      
  
        if(1){    
            if(checkTransport('T', 9, docks)==0){
                pthread_kill(docks->trucksSharedDock.trs[9].tid, SIGUSR1);
                docks->trucksSharedDock.trs[9].dest = 0;
                vehicleGone = 1;
                infoMsg.vehicleType = 'T';
                if (msgsnd(getMsgid(), &infoMsg, sizeof(infoMsg),0) == -1) {
                        perror("Msgsnd error");
                        exit(1);
                }
            }else{
                //Will come back in the queue
                pthread_kill(docks->trucksSharedDock.trs[9].tid, SIGUSR1);
            }


            if(checkTransport('t', 1, docks)==0){
                pthread_kill(docks->trainSharedDock.trs[1].tid, SIGUSR1);
                docks->trainSharedDock.trs[1].dest = 0;
                vehicleGone = 1;
                infoMsg.vehicleType = 't';
                if (msgsnd(getMsgid(), &infoMsg, sizeof(infoMsg),0) == -1) {
                        perror("Msgsnd error");
                        exit(1);
                }
            }

            if(checkTransport('b', 0, docks)==0){
                pthread_kill(docks->boatSharedDock.trs[0].tid, SIGUSR1);
                docks->boatSharedDock.trs[0].dest = 0;
                vehicleGone = 1;
                infoMsg.vehicleType = 'b';
                if (msgsnd(getMsgid(), &infoMsg, sizeof(infoMsg),0) == -1) {
                        perror("Msgsnd error");
                        exit(1);
                }
            }

            if(checkTransport('b', 1, docks)==0){
                pthread_kill(docks->boatSharedDock.trs[1].tid, SIGUSR1);
                docks->boatSharedDock.trs[1].dest = 0;
                vehicleGone = 1;
                infoMsg.vehicleType = 'b';
                if (msgsnd(getMsgid(), &infoMsg, sizeof(infoMsg),0) == -1) {
                        perror("Msgsnd error");
                        exit(1);
                }
            }
            

            if(vehicleGone){
                //Warn the generator to generate a new transport of the same type as the one gone
                pthread_kill(c->genTransport, SIGUSR1);
            }
            

        }

        printShmem(getShmid());
        unlock(FULL);
        vehicleGone = 0;

        pthread_cond_signal(&nextCrane);

        pthread_mutex_lock(&nextCraneMut);
        pthread_cond_wait(&nextCrane, &nextCraneMut);
        pthread_mutex_unlock(&nextCraneMut);

    }

    //kills all transport
    if (cleaned==0){
        pthread_kill(docks->trucksSharedDock.trs[9].tid, SIGINT);
        pthread_kill(docks->trainSharedDock.trs[1].tid, SIGUSR1);
        pthread_kill(docks->boatSharedDock.trs[0].tid, SIGUSR1);
        pthread_kill(docks->boatSharedDock.trs[1].tid, SIGUSR1);
        sleep(2);
        pthread_kill(c->genTransport, SIGINT);
        cleaned = 1;
    }

    pthread_cond_signal(&nextCrane);
    pthread_exit(0);



}

