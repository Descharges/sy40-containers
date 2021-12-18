#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>


void generateTrains(){
    pthread_t thread[2];

    trainAndCommunication trainAndCom[2];

    //Create the shared memory useful to check if a train can move forward
    int shmid;
    bool *topPositionOccupied;
    shmid = shmget(IPC_PRIVATE, sizeof(int), 0666);
    topPositionOccupied= (bool *)shmat(shmid, NULL, 0);
    *topPositionOccupied = false;	//initialisation � 0 du compteur
    printf("False :%s\n", *topPositionOccupied? "tr":"false" );

    //Train thread creation
    for(int  i = 0 ; i<2 ; i++){//Finite loop for TEST purpose
        trainAndCom[i].train.type = 't';
        trainAndCom[i].train.id = i;
        trainAndCom[i].topPositionOccupied = topPositionOccupied;
        
        if (pthread_create(thread+i, 0,(void *) trainArrival, &trainAndCom[i]) != 0)
	        perror("Erreur Creation thread");

        
        
    }

    //Train thread destruction 
    for(int i=0;i<2;i++){
            pthread_join(thread[i],NULL);
        }
}