#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>


#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited
#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited

//beaucoup de nettoyage a faire ici
//oui

void crane(){

}


//For TEST purpose, should probably be with the truck and boat generation
void generateTrains(){

    pthread_t thread[TOTAL_NUMBER_OF_BOATS];

    //Create the shared memory of the train dock
    int shmid;
    Dtrains *trainSharedDock;
    shmid = shmget(IPC_PRIVATE, sizeof(int), 0666);
    trainSharedDock = (Dtrains *)shmat(shmid, NULL, 0);
    trainSharedDock->trs[0] = -1;
    trainSharedDock->trs[1] = -1;
    for(int i = 0 ; i<NB_OF_BOATS ; i++)
        trainSharedDock->cont[i] = -1;

    
    //Train thread creation
    for(int  i = 0 ; i<TOTAL_NUMBER_OF_BOATS ; i++){//Finite loop for TEST purpose
        transport* trainToGenerate = malloc(sizeof(transport));
        trainToGenerate->shmem.trainSharedDock = trainSharedDock;
        trainToGenerate->type = 't';
        trainToGenerate->id = i;
        if (pthread_create(thread+i, 0,(void *) train, trainToGenerate) != 0)
	        perror("Erreur Creation thread");
    }

    //Train thread destruction
    for(int i=0;i<TOTAL_NUMBER_OF_BOATS;i++){
            pthread_join(thread[i],NULL);
        }
}


//For TEST purpose, should probably be with the truck and train generation
void generateBoats(){

    pthread_t thread[TOTAL_NUMBER_OF_BOATS];

    //Create the shared memory of the boat dock
    int shmid;
    Dboats *boatSharedDock;
    shmid = shmget(IPC_PRIVATE, sizeof(int), 0666);
    boatSharedDock = (Dboats *)shmat(shmid, NULL, 0);
    boatSharedDock->trs[0] = -1;
    boatSharedDock->trs[1] = -1;
    for(int i = 0 ; i<NB_OF_BOATS ; i++)
        boatSharedDock->cont[i] = -1;

    
    //Train thread creation
    for(int  i = 0 ; i<TOTAL_NUMBER_OF_BOATS ; i++){//Finite loop for TEST purpose
        transport* boatToGenerate = malloc(sizeof(transport));
        boatToGenerate->shmem.boatSharedDock = boatSharedDock;
        boatToGenerate->type = 't';
        boatToGenerate->id = i;
        if (pthread_create(thread+i, 0,(void *) boat, boatToGenerate) != 0)
	        perror("Erreur Creation thread");
    }

    //Train thread destruction
    for(int i=0;i<TOTAL_NUMBER_OF_BOATS;i++){
            pthread_join(thread[i],NULL);
        }
}