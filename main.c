#include "main.h"
#include "transportGeneration.h"
#include "docks.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>




void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

int main(){
  //create the shared memory struct
  shmemInit();
  Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);
  memset(docks, -1, sizeof(Docks));


  printShmem(getShmid());

  genCrane();
  genTerrain();
  genTransport(docks);
  return 0;
}

void genCrane(){


}

void genTerrain(){
  printf("Terrain generated with TID=%d\n",2);
}

void genTransport(){
  printf("Generating transport methods...\n");

  generateBoats();



  //Generate trucks
  /*pthread_t thread[20];

  for(int i=0;i<20;i++){
    transport* t = malloc(sizeof(transport));
    t->shmem = docks;
    t->type = 'T';
    t->id = i;
    pthread_create(thread+i,0,(void *)transportFunc, t);
  }

  for(int i=0;i<20;i++){
    pthread_join(thread[i],NULL);
  }*/


}
