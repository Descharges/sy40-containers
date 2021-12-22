#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>


#define TOTAL_NUMBER_OF_TRAINS 4//For TEST purpose, it should be unlimited

//beaucoup de nettoyage a faire ici
//oui

void crane(){

}



void generateTrains(){
    pthread_t thread[TOTAL_NUMBER_OF_TRAINS];

    trainAndCommunication trainAndCom[TOTAL_NUMBER_OF_TRAINS];

    //Create the shared memory useful to check if a train can move forward
    int shmid;

    //Create the shared memory of the train dock
    int shmid2;
    Dtrains *trainSharedDock;
    shmid = shmget(IPC_PRIVATE, sizeof(int), 0666);
    trainSharedDock= (Dtrains *)shmat(shmid, NULL, 0);
    trainSharedDock->trs[0] = -1;
    trainSharedDock->trs[1] = -1;
    for(int i = 0 ; i<NB_OF_TRAINS ; i++)
        trainSharedDock->cont[i] = -1;

        
    //Train thread creation
    for(int  i = 0 ; i<TOTAL_NUMBER_OF_TRAINS ; i++){//Finite loop for TEST purpose
        trainAndCom[i].train.type = 't';
        trainAndCom[i].train.id = i;
        trainAndCom[i].sharedDock = trainSharedDock;
    if (pthread_create(thread+i, 0,(void *) train, &trainAndCom[i]) != 0)
	        perror("Erreur Creation thread");
    }

    //Train thread destruction
    for(int i=0;i<TOTAL_NUMBER_OF_TRAINS;i++){
            pthread_join(thread[i],NULL);
        }
}
