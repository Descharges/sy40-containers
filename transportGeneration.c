#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>


#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited
#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited


void crane(){

}


//For TEST purpose, should probably be with the truck and boat generation
void generateTrains(){

    pthread_t thread[20];


    //Train thread creation
    for(int  i = 0 ; i<20; i++){//Finite loop for TEST purpose
        transport* t = malloc(sizeof(transport));
        t->type = 't';
        t->id = i+1;
        t->dest = 'A';
        t->shmid = getShmid();
        if (pthread_create(thread+i, 0,(void *) transportFunc, t) != 0)
	        perror("Erreur Creation thread");
    }

    //Train thread destruction
    for(int i=0;i<20;i++){
            pthread_join(thread[i],NULL);
        }
}


//For TEST purpose, should probably be with the truck and train generation
void generateBoats(){

    pthread_t thread[TOTAL_NUMBER_OF_BOATS];

    //Train thread creation
    for(int  i = 0 ; i<TOTAL_NUMBER_OF_BOATS ; i++){//Finite loop for TEST purpose
        fflush(stdout);
        transport* t = malloc(sizeof(transport));

        t->type = 'b';
        t->id = i+1;
        t->dest = 'A';
        t->shmid = getShmid();
        if (pthread_create(thread+i, 0,(void *) transportFunc, t) != 0){
	        perror("Erreur Creation thread");
        }
    }

    Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);


    //Train thread destruction
    for(int i=0;i<TOTAL_NUMBER_OF_BOATS;i++){
        pthread_join(thread[i],NULL);
    }
}

void generateTrucks(){

    pthread_t thread[20];

    //Train thread creation
    for(int  i = 0 ; i<20 ; i++){//Finite loop for TEST purpose
        fflush(stdout);
        transport* t = malloc(sizeof(transport));

        t->type = 'T';
        t->id = i+1;
        t->dest = 'A';
        t->shmid = getShmid();
        if (pthread_create(thread+i, 0,(void *) transportFunc, t) != 0){
	        perror("Erreur Creation thread");
        }
    }

    Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);


    //Train thread destruction
    for(int i=0;i<20;i++){
        pthread_join(thread[i],NULL);
    }
}