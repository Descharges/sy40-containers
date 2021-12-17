#include "transportGeneration.h"
#include "transport.h"

#include <pthread.h>


void generateTrains(){
    pthread_t thread[2];

    transport train[2];

    
    //Train thread creation
    for(int  i = 0 ; i<2 ; i++){//Finite loop for test purpose
        train[i].type = 't';
        train[i].id = i;
        
        if (pthread_create(thread+i, 0,(void *) trainArrival, &train[i]) != 0)
	        perror("Erreur Creation thread");

        
        
    }

    //Train thread destruction 
    for(int i=0;i<20;i++){
            pthread_join(thread[i],NULL);
        }
}