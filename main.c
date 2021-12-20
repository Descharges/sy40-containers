#include "main.h"
#include "transportGeneration.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

void main(){
  //create the shared memory struct

  genCrane();
  genTerrain();
  genTransport(docks);
}

void genCrane(){
  printf("Crane generated with TID=%d\n",1);
}

void genTerrain(){
  printf("Terrain generated with TID=%d\n",2);
}

void genTransport(Docks* docks){
  printf("Generating transport methods...\n");

  //generateTrains();

  //while(1){}


  pthread_t thread[20];

  for(int i=0;i<20;i++){
    transport* t = malloc(sizeof(transport));
    t->shmem = docks;
    t->type = 'T';
    t->id = i;
    pthread_create(thread+i,0,(void *)transportFunc, t);
  }

  for(int i=0;i<20;i++){
    pthread_join(thread[i],NULL);
  }


}
