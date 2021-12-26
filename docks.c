#include "docks.h"

pthread_mutex_t trainMutexShm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t truckMutexShm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t boatMutexShm = PTHREAD_MUTEX_INITIALIZER;

int shmid;

void shmemInit(){
  printf("Allocating shared memory\n");
  shmid = shmget(IPC_PRIVATE, sizeof(Docks), 0666);
}

int getShmid(){
  return shmid;
}

void printShmem(int shmid){

  Docks* shmem = (Docks *)shmat(getShmid(), NULL, 0);
  printf("Appercu dock :\n");
  printf("Trucks:\n");
  for (int i=0; i<NB_OF_TRUCKS; i++){
    printf("[%d]",shmem->trucksSharedDock.trs[i]);
  }
  printf("\n");

  printf("Trucks container:\n");
  for (int i=0; i<NB_CONTAINER_TRUCK; i++){
    printf("[%d]",shmem->trucksSharedDock.cont[i]);
  }
  printf("\n");

  printf("Boat:\n");
  for (int i=0; i<NB_OF_BOATS; i++){
    printf("[%d]",shmem->boatSharedDock.trs[i]);
  }
  printf("\n");

  printf("Boat container:\n");
  for (int i=0; i<NB_CONTAINER_BOAT; i++){
    printf("[%d]",shmem->boatSharedDock.cont[i]);
  }
  printf("\n");

  printf("Train:\n");
  for (int i=0; i<NB_OF_TRAINS; i++){
    printf("[%d]",shmem->trainSharedDock.trs[i]);
  }
  printf("\n");

  printf("Train container:\n");
  for (int i=0; i<NB_CONTAINER_TRAIN; i++){
    printf("[%d]",shmem->trainSharedDock.cont[i]);
  }
  printf("\n");

}

void lock(int arg){
  switch(arg){
    case TRAIN:
      pthread_mutex_lock(&trainMutexShm);
      break;
    case TRUCK:
      pthread_mutex_lock(&truckMutexShm);
      break;
    case BOAT:
      pthread_mutex_lock(&boatMutexShm);
      break;
    case FULL:
      pthread_mutex_lock(&trainMutexShm);
      pthread_mutex_lock(&truckMutexShm);
      pthread_mutex_lock(&boatMutexShm);
      break;
  }
}

void unlock(int arg){
  switch(arg){
    case TRAIN:
      pthread_mutex_unlock(&trainMutexShm);
      break;
    case TRUCK:
      pthread_mutex_unlock(&truckMutexShm);
      break;
    case BOAT:
      pthread_mutex_unlock(&boatMutexShm);
      break;
    case FULL:
      pthread_mutex_unlock(&trainMutexShm);
      pthread_mutex_unlock(&truckMutexShm);
      pthread_mutex_unlock(&boatMutexShm);
      break;
  }
}
