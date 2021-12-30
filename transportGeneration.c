#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>


#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited
#define TOTAL_NUMBER_OF_BOATS 4//For TEST purpose, it should be unlimited


void crane(){

}

//For TEST purpose
void generateTrains(){

    pthread_t thread[20];


    //Train thread creation
    for(int  i = 0 ; i<20; i++){//Finite loop for TEST purpose
        transport* t = malloc(sizeof(transport));

        container* c = malloc(sizeof(container)*5);

        t->type = 't';
        t->id = i;
        t->dest = 'A';
        t->shmid = getShmid();


        //TODO: J'ai fait ca de manière très très sale, a corriger
        c[0].id = (i*10);
        c[1].id = (i*10)+1;
        c[2].id = (i*10)+2;
        c[3].id = (i*10)+3;
        c[4].id = (i*10)+4;
        c[0].dest = 'A';
        c[1].dest = 'A';
        c[2].dest = 'A';
        c[3].dest = 'A';
        c[4].dest = 'A';

        t->contArray = c;

        if (pthread_create(thread+i, 0,(void *) transportFunc, t) != 0)
	        perror("Erreur Creation thread");
    }

    //Train thread destruction
    for(int i=0;i<20;i++){
        pthread_join(thread[i],NULL);
    }
}


//For TEST purpose
void generateBoats(){

    pthread_t thread[TOTAL_NUMBER_OF_BOATS];

    //Train thread creation

    for(int  i = 0 ; i<TOTAL_NUMBER_OF_BOATS ; i++){//Finite loop for TEST purpose
        
        transport* t = malloc(sizeof(transport));

        container* c = malloc(sizeof(container)*3);

        t->type = 'b';
        t->id = i;
        t->dest = 'A';
        t->shmid = getShmid();
        c[0].id = (i*10);
        c[1].id = (i*10)+1;
        c[2].id = (i*10)+2;
        c[0].dest = 'A';
        c[1].dest = 'A';
        c[2].dest = 'A';
        t->contArray = c;

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

    pthread_t thread[2];

    //Train thread creation
    for(int  i = 0 ; i<2 ; i++){//Finite loop for TEST purpose
        fflush(stdout);
        transport* t = malloc(sizeof(transport));

        container* c = malloc(sizeof(container));

        t->type = 'T';
        t->id = i+1;
        t->dest = 'A';
        t->shmid = getShmid();
        c->id = i-1;
        c->dest = 'A';
        t->contArray = c;

        if (pthread_create(thread+i, 0,(void *) transportFunc, t) != 0){
	        perror("Erreur Creation thread");
        }
    }

    Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);

    //generateTrains();


    //Train thread destruction
    //for(int i=0;i<2;i++){
    //    pthread_join(thread[i],NULL);
    //}
}