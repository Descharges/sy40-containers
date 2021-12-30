#include "transport.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>


//===Mutex to handle the access to the docks
//Train
pthread_cond_t trainWaitingQueue = PTHREAD_COND_INITIALIZER;//For the other trains
pthread_mutex_t trainMutex = PTHREAD_MUTEX_INITIALIZER;
int nTrains = 0;
pthread_cond_t trainsAdv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t advTrainMutex = PTHREAD_MUTEX_INITIALIZER;

//Boat
pthread_cond_t boatWaitingQueue = PTHREAD_COND_INITIALIZER;//For the other trains
pthread_mutex_t boatMutex = PTHREAD_MUTEX_INITIALIZER;
int nBoats = 0;

//Truck
pthread_cond_t truckWaitingQueue = PTHREAD_COND_INITIALIZER;//For the other trains
pthread_mutex_t truckMutex = PTHREAD_MUTEX_INITIALIZER;
int nTrucks = 0;
pthread_cond_t truckAdv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t advMutex = PTHREAD_MUTEX_INITIALIZER;
int truckTurn = 0;
pthread_mutex_t mutexTruckTurn = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t nBoatMutex = PTHREAD_MUTEX_INITIALIZER;

container nullContainer;



void transportFunc(transport* t){
  nullContainer.id = -1;
  switch (t->type){
    case 'b'://boat
      boat(t);
      break;
    case 't'://train
      train(t);
      break;
    case 'T'://Truck (maybe find a better letter ? idk)
      truck(t);
      break;
  }
}

void sigHandler(int signo){
  printf("=========Signal handler triggered\n");
  exit(0);
}

void boat(transport *t){

  //TODO: remettre le booléen



  //Set the interupt action
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sigHandler;
  sigemptyset (&unpauseSigaction.sa_mask);
  unpauseSigaction.sa_flags = 0;
  sigaction(SIGINT, &unpauseSigaction, NULL);

  //get the pointer to the struct
  Docks* docks = (Docks *)shmat(t->shmid, NULL, 0);
  Dboats *boatsDock = &(docks->boatSharedDock);




  printf("[BOAT %d]arriving...\n", t->id);

  //If there is no room available on the docks, come in the waiting queue

  //Accessing the dock using nBoats variable
  pthread_mutex_lock(&boatMutex);
  if (nBoats >= 2){
    printf("[BOAT %d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&boatWaitingQueue,&boatMutex);
  }
  printf("[BOAT %d]Entering the docks with %d,%d,%d\n", t->id, t->contArray[0].id,t->contArray[1].id,t->contArray[2].id);
  nBoats++;
  pthread_mutex_unlock(&boatMutex);

  //setting itself up in shared memory
  lock(BOAT);
  if(boatsDock->trs[0] == -1){
    boatsDock->trs[0] = t->id;
    t->pos = 0;
    boatsDock->cont[0] = t->contArray[0];
    boatsDock->cont[1] = t->contArray[1];
    boatsDock->cont[2] = t->contArray[2];
  }else{
    boatsDock->trs[1] = t->id;
    t->pos = 1;
    boatsDock->cont[3] = t->contArray[0];
    boatsDock->cont[4] = t->contArray[1];
    boatsDock->cont[5] = t->contArray[2];
  }
  unlock(BOAT);


  //Waiting for whatever is telling the boat to leave
  //sleep(3);

  //removing itself from shared mem
  lock(BOAT);
  boatsDock->trs[t->pos] = -1;
  t->contArray[0] = boatsDock->cont[t->pos * 3];
  t->contArray[1] = boatsDock->cont[(t->pos * 3)+1];
  t->contArray[2] = boatsDock->cont[(t->pos * 3)+2];
  boatsDock->cont[t->pos * 3] = nullContainer;
  boatsDock->cont[(t->pos * 3)+1] = nullContainer;
  boatsDock->cont[(t->pos * 3)+2]  = nullContainer;

  //TODO: idk
  unlock(BOAT);

  //Lowering nBoats and sending signal
  pthread_mutex_lock(&boatMutex);
  nBoats--;
  pthread_mutex_unlock(&boatMutex);
  pthread_cond_signal(&boatWaitingQueue);

  printf("[BOAT %d]Leaving the docks with %d,%d,%d\n", t->id,t->contArray[0].id,t->contArray[1].id,t->contArray[2].id);


  //Mettre le tout dans une boucle while
  //libérer le malloc fait précédemment
  free(t->contArray);
  free(t);
}



void truck(transport *t){

  int turn = 0;

  //Set sigaction
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sigHandler;
  sigemptyset (&unpauseSigaction.sa_mask);
  unpauseSigaction.sa_flags = 0;
  sigaction(SIGINT, &unpauseSigaction, NULL);

  //get the pointer to the struct
  Docks* docks = (Docks *)shmat(t->shmid, NULL, 0);
  Dtrucks *trucksDock = &(docks->trucksSharedDock);

  printf("[TRUCK %d]arriving...\n", t->id);

  pthread_mutex_lock(&truckMutex);
  if (nTrucks >= 10){
    printf("[TRUCK %d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&truckWaitingQueue,&truckMutex);
    t->pos = 9-nTrucks;
  }else{
    t->pos = 9-nTrucks;
  }
  nTrucks++;
  printf("[TRUCK %d]Entering the docks at pos %d with container %d\n", t->id, t->pos, t->contArray->id);
  pthread_mutex_unlock(&truckMutex);

  lock(TRUCK);
  trucksDock->trs[t->pos] = t->id; 
  trucksDock->cont[t->pos] = *(t->contArray);
  unlock(TRUCK);

  while (t->pos < 9){
    //wait for the signal to advance
    pthread_mutex_lock(&advMutex);
    pthread_cond_wait(&truckAdv, &advMutex);
    pthread_mutex_unlock(&advMutex);

    
    do{
      pthread_mutex_lock(&mutexTruckTurn);
      turn = truckTurn;
      pthread_mutex_unlock(&mutexTruckTurn);
    }while(turn != t->pos);


    lock(TRUCK);
    trucksDock->trs[t->pos] = -1;
    if(t->pos == 0){
      pthread_cond_signal(&truckWaitingQueue);
    }
    t->pos++;
    trucksDock->trs[t->pos] = t->id;
    trucksDock->cont[t->pos] = trucksDock->cont[t->pos - 1];
    trucksDock->cont[t->pos - 1] = nullContainer;
    unlock(TRUCK);
    printf("[TRUCK %d]Moving forward to pos %d\n", t->id, t->pos);

    pthread_mutex_lock(&mutexTruckTurn);
    truckTurn--;
    lock(TRUCK);
    while(trucksDock->trs[truckTurn] == -1 && truckTurn!=0){
      truckTurn--;
    } 
    unlock(TRUCK);
    pthread_mutex_unlock(&mutexTruckTurn);
  }

  //waiting to get order to move forward
  sleep(60);

  pthread_mutex_lock(&truckMutex);
  nTrucks--;
  pthread_mutex_unlock(&truckMutex);

  pthread_mutex_lock(&mutexTruckTurn);
  truckTurn = 8;
  pthread_mutex_unlock(&mutexTruckTurn);

  lock(TRUCK);
  trucksDock->trs[t->pos] = -1;
  *(t->contArray) = trucksDock->cont[t->pos];
  trucksDock->cont[t->pos] = nullContainer;
  unlock(TRUCK);


  printf("[TRUCK %d]Leaving with  container %d\n", t->id, t->contArray->id);

  pthread_cond_broadcast(&truckAdv);

  free(t->contArray);
  free(t);
}



void train(transport *t){

  //Set sigaction
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sigHandler;
  sigemptyset (&unpauseSigaction.sa_mask);
  unpauseSigaction.sa_flags = 0;
  sigaction(SIGINT, &unpauseSigaction, NULL);

  //get the pointer to the struct
  Docks* docks = (Docks *)shmat(t->shmid, NULL, 0);
  Dtrains *trainDock = &(docks->trainSharedDock);

  printf("[TRAIN %d]arriving...\n", t->id);

  pthread_mutex_lock(&trainMutex);
  if (nTrains >= 2){
    printf("[TRAIN %d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&trainWaitingQueue,&trainMutex);
    t->pos = 1-nTrains;
  }else{
    t->pos = 1-nTrains;
  }
  nTrains++;
  printf("[TRAIN %d]Entering the docks at pos %d with %d,%d,%d,%d,%d\n", t->id, t->pos, t->contArray[0].id, t->contArray[1].id, t->contArray[2].id, t->contArray[3].id, t->contArray[4].id);
  pthread_mutex_unlock(&trainMutex);

  lock(TRAIN);
  trainDock->trs[t->pos] = t->id;
  for(int i=0; i<5;i++){
    trainDock->cont[((t->pos)*5)+i] = t->contArray[i];
  }
  unlock(TRAIN);

  while (t->pos < 1){
    pthread_mutex_lock(&advTrainMutex);
    pthread_cond_wait(&trainsAdv, &advTrainMutex);
    pthread_mutex_unlock(&advTrainMutex);
    lock(TRAIN);
    trainDock->trs[t->pos] = -1;
    if(t->pos == 0){
      pthread_cond_signal(&trainWaitingQueue);
    }
    t->pos++;
    trainDock->trs[t->pos] = t->id;
    for(int i=0; i<5;i++){
      trainDock->cont[(t->pos *5)+i] = trainDock->cont[((t->pos -1)*5)+i];
      trainDock->cont[((t->pos -1)*5)+i] = nullContainer;
    }
    unlock(TRAIN);
    printf("[TRAIN %d]Moving forward to pos %d\n", t->id, t->pos);
  }

  //waiting to get order to move forward
  sleep(3);

  pthread_mutex_lock(&trainMutex);
  nTrains--;
  pthread_mutex_unlock(&trainMutex);

  lock(TRAIN);
  trainDock->trs[t->pos] = -1;
  for(int i=0; i<5;i++){
    t->contArray[i] = trainDock->cont[((t->pos)*5)+i];
    trainDock->cont[((t->pos)*5)+i] = nullContainer;
  }
  
  unlock(TRAIN);

   printf("[TRAIN %d]Leaving with %d,%d,%d,%d,%d\n", t->id, t->contArray[0].id, t->contArray[1].id, t->contArray[2].id, t->contArray[3].id, t->contArray[4].id);

  pthread_cond_broadcast(&trainsAdv);

  free(t->contArray);
  free(t);
}
