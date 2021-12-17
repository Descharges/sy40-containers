#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>


void generateTrains(){
    int thread_id[2];

    transport train[2];
    train[0].type = 't';
    train[0].id = 1;

    train[1].type = 't';
    train[1].id = 2;

    for(int  i = 0 ; i<2 ; i++){//Finite loop for test purpose
        if (pthread_create(&thread_id[i], 0,(void *) trainArrival, &train[i]) != 0)
	        perror("Erreur Creation thread");
	      
    }
}