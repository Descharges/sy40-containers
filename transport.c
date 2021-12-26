#include "transport.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

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
//pthread_mutex_t nBoatMutex = PTHREAD_MUTEX_INITIALIZER;


void transportFunc(transport* t){
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

void sig_handler(int signo){
  printf("Signal handler triggered\n");
}

void boat(transport *t){

  //TODO: remettre le booléen

  //Set the interupt action
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sig_handler;
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
  printf("[BOAT %d]Entering the docks\n", t->id);
  nBoats++;
  pthread_mutex_unlock(&boatMutex);

  //setting itself up in shared memory
  lock(BOAT);
  if(boatsDock->trs[0] == -1){
    boatsDock->trs[0] = t->id;
    t->pos = 0;
  }else{
    boatsDock->trs[1] = t->id;
    t->pos = 1;
  }
  unlock(BOAT);


  //Waiting for whatever is telling the boat to leave
  //sleep(3);

  printf("[BOAT %d]Leaving the docks\n", t->id);

  //removing itself from shared mem
  lock(BOAT);
  boatsDock->trs[t->pos] = -1;
  //TODO: container cleaning
  unlock(BOAT);

  //Lowering nBoats and sending signal
  pthread_mutex_lock(&boatMutex);
  nBoats--;
  pthread_mutex_unlock(&boatMutex);
  pthread_cond_signal(&boatWaitingQueue);

  //Mettre le tout dans une boucle while
  //libérer le malloc fait précédemment
  free(t);
}

void truck(transport *t){

  //Set sigaction
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sig_handler;
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
  printf("[TRUCK %d]Entering the docks at pos %d\n", t->id, t->pos);
  pthread_mutex_unlock(&truckMutex);

  lock(TRUCK);
  trucksDock->trs[t->pos] = t->id; 
  unlock(TRUCK);

  while (t->pos < 9){
    pthread_mutex_lock(&advMutex);
    pthread_cond_wait(&truckAdv, &advMutex);
    pthread_mutex_unlock(&advMutex);
    lock(TRUCK);
    trucksDock->trs[t->pos] = -1;
    if(t->pos == 0){
      pthread_cond_signal(&truckWaitingQueue);
    }
    t->pos++;
    trucksDock->trs[t->pos] = t->id;
    unlock(TRUCK);
    printf("[TRUCK %d]Moving forward to pos %d\n", t->id, t->pos);
  }

  //waiting to get order to move forward
  sleep(3);

  printf("[TRUCK %d]Leaving...\n", t->id);

  pthread_mutex_lock(&truckMutex);
  nTrucks--;
  pthread_mutex_unlock(&truckMutex);

  lock(TRUCK);
  trucksDock->trs[t->pos] = -1;
  unlock(TRUCK);

  pthread_cond_broadcast(&truckAdv);
  free(t);
}

void train(transport *t){

  //Set sigaction
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sig_handler;
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
  printf("[TRAIN %d]Entering the docks at pos %d\n", t->id, t->pos);
  pthread_mutex_unlock(&trainMutex);

  lock(TRAIN);
  trainDock->trs[t->pos] = t->id; 
  printf("OK");
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
    unlock(TRAIN);
    printf("[TRAIN %d]Moving forward to pos %d\n", t->id, t->pos);
  }

  //waiting to get order to move forward
  sleep(3);

  printf("[TRAIN %d]Leaving...\n", t->id);

  pthread_mutex_lock(&trainMutex);
  nTrains--;
  pthread_mutex_unlock(&trainMutex);

  lock(TRAIN);
  trainDock->trs[t->pos] = -1;
  unlock(TRAIN);

  pthread_cond_broadcast(&trainsAdv);
  free(t);
}
