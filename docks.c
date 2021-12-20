#include "docks.h"

pthread_mutex_t train = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t truck = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t boat = PTHREAD_MUTEX_INITIALIZER;

int shmid;

Docks* init(){
  shmid = shmget(IPC_PRIVATE, sizeof(struct Docks), 0666);
  return (Docks *)shmat(shmid, NULL, 0);
}

void lock(int arg){
  switch(arg){
    case TRAIN:
      pthread_mutex_lock(&train);
      break;
    case TRUCK:
      pthread_mutex_lock(&truck);
      break;
    case BOAT:
      pthread_mutex_lock(&boat);
      break;
    case FULL:
      pthread_mutex_lock(&train);
      pthread_mutex_lock(&truck);
      pthread_mutex_lock(&boat);
      break;
  }
}

void unlock(int arg){
  switch(arg){
    case TRAIN:
      pthread_mutex_unlock(&train);
      break;
    case TRUCK:
      pthread_mutex_unlock(&truck);
      break;
    case BOAT:
      pthread_mutex_unlock(&boat);
      break;
    case FULL:
      pthread_mutex_unlock(&train);
      pthread_mutex_unlock(&truck);
      pthread_mutex_unlock(&boat);
      break;
  }
}
