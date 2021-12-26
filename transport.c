#include "transport.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

//===Mutex to handle the access to the docks
//Train
pthread_mutex_t trainMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t trainDockQueue = PTHREAD_COND_INITIALIZER,//Contain only 2 trains
trainWaitingQueue = PTHREAD_COND_INITIALIZER;//For the other trains

//Boat
pthread_cond_t boatWaitingQueue = PTHREAD_COND_INITIALIZER;//For the other trains
pthread_mutex_t boatMutex = PTHREAD_MUTEX_INITIALIZER;

int nBoats = 0;
//pthread_mutex_t nBoatMutex = PTHREAD_MUTEX_INITIALIZER;


void transportFunc(transport* t){
  switch (t->type){
    case 'b'://boat
      boat(t);
      break;
    case 't'://train
      //train(t);
      break;
    case 'T'://Truck (maybe find a better letter ? idk)
      //truck(t);
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


/*

void train(transport *t){

  printf("Train %d arrive\n", t->id);

  bool filledWithGoodDestination = true;//Should be set to FALSE at the beginning
  bool isOnTopPosition = false;
  bool goInWaitingQueue = true;

  transport train = *((transport*)(t));

  //If there is no room available on the docks, come in the waiting queue
  pthread_mutex_lock(&trainMutex);
    //A retravailler pour la scalabilité
    for(int i = 0 ; i<NB_OF_TRAINS ; i++){
      if(train.shmem.trainSharedDock->trs[i]<0){
        goInWaitingQueue = false;
        break;
      }
    }
    if(goInWaitingQueue){
      pthread_cond_wait(&trainWaitingQueue, &trainMutex);
    }
    if(train.shmem.trainSharedDock.trs[0]>0 && train.shmem.trainSharedDock.trs[1]>0){
      pthread_cond_wait(&trainWaitingQueue, &trainMutex);
    }
  pthread_mutex_unlock(&trainMutex);

  printf("COUCOU");
  //==Give the train its initial position
  pthread_mutex_lock(&trainMutex);
    for(int i = 0 ; i<NB_OF_TRAINS ; i++){
        if(train.shmem.trainSharedDock.trs[i]<0){
          train.shmem.trainSharedDock.trs[i] = train.id;
          if(i==0)
            isOnTopPosition = true;
          break;
        }
      }
  pthread_mutex_unlock(&trainMutex);

  //For TEST purpose
  if(train.id == 0){
  }else if(train.id == 3){
    pthread_cond_signal(&trainDockQueue);
  }

  //===The train wait
  pthread_mutex_lock(&trainMutex);
    pthread_cond_wait(&trainDockQueue, &trainMutex);
  pthread_mutex_unlock(&trainMutex);
  sleep(1);

  //===Check if the train should move
    pthread_mutex_lock(&trainMutex);
      //If there is no train on top position
      if(train.shmem.trainSharedDock.trs[0] < 0){
        //move to the top position
        isOnTopPosition = true;
        train.shmem.trainSharedDock.trs[0] = train.id;
        train.shmem.trainSharedDock.trs[1] = -1;
        printf("Train %d take the top position\n", train.id);
      }
    pthread_mutex_unlock(&trainMutex);

    printf("Train occupying top position : %d ; Train occupying mid position : %d\n", train.shmem.trainSharedDock.trs[0], train.shmem.trainSharedDock.trs[1]);

    if(filledWithGoodDestination && isOnTopPosition){
      //Leave (= destruction) TO DO
      printf("Train %d leave\n", train.id);
      //Give signal to the next train on the dock to come
      pthread_mutex_lock(&trainMutex);
        train.shmem.trainSharedDock.trs[0] = -1;
        pthread_cond_signal(&trainDockQueue);
      pthread_mutex_unlock(&trainMutex);
      //Give signal for the next train in the waiting queue to come
      pthread_mutex_lock(&trainMutex);
        pthread_cond_signal(&trainWaitingQueue);
      pthread_mutex_unlock(&trainMutex);
    }
}




void truck(transport* t){
  //Wait for signal that a case is available for him
  pthread_mutex_lock(&truckMutex);
  if (nTrucks < 10){
    t->pos = 10-nTrucks;
  }else{
    //printf("[TRUCK ID=%d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&truckQueue, &truckMutex);
    t->pos = 1;
  }
  nTrucks++;
  pthread_mutex_unlock(&truckMutex);

  //Change his value in the row
  pthread_mutex_lock(&posMutex);
  t->shmem->cdocks.trucks[(t->pos)-1] = t->id;
  pthread_mutex_unlock(&posMutex);

  //printf("[TRUCK ID=%d]The truck entered the dock at position %d\n",t->id,t->pos);

  while(t->pos != 10){

    pthread_mutex_lock(&advMutex);
    pthread_cond_wait(&truckAdv, &advMutex);
    pthread_mutex_unlock(&advMutex);

    t->pos++;
    pthread_mutex_lock(&posMutex);
    t->shmem->cdocks.trucks[(t->pos)-1] = t->id;
    pthread_mutex_unlock(&posMutex);
    if(t->pos == 2){
      pthread_cond_signal(&truckQueue);
    }
    //printf("[TRUCK ID=%d]The truck move forward to position %d\n",t->id,t->pos);
  }

  sleep(1); //Here goes the dicision to go or not

  //I should secure all thoses prints but :
  //1-Prints are read only
  //2-It's only debug and won't stay
  //3-I'm lazy

  printf("\e[1;1H\e[2J");
  printf("[TRUCK ID=%d]The truck is leaving...\n",t->id);
  printf("[TRUCK ID=%d]Number of trucks =%d\n",t->id,nTrucks);
  for(int i=0;i<10;i++){
    printf("-[%d]-",t->shmem->cdocks.trucks[i]);
  }
  printf("\n\n");


  pthread_mutex_lock(&truckMutex);
  nTrucks--;
  pthread_mutex_unlock(&truckMutex);

  //reset the array to avoid duplicates
  pthread_mutex_lock(&posMutex);
  memset(t->shmem->cdocks.trucks, 0, sizeof t->shmem->cdocks.trucks);
  pthread_mutex_unlock(&posMutex);
  pthread_cond_broadcast(&truckAdv);

  free(t);
  pthread_exit(NULL);
}
*/
