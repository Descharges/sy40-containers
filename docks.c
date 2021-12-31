#include "docks.h"
#include <sys/msg.h>

pthread_mutex_t trainMutexShm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t truckMutexShm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t boatMutexShm = PTHREAD_MUTEX_INITIALIZER;

int shmid;
int msgid;

void shmemInit(){
  printf("Allocating shared memory\n");
  shmid = shmget(IPC_PRIVATE, sizeof(Docks), 0666);
}
void msgQInit(){
    
  key_t cle;
  if ((cle = ftok("./", 'A')) == -1) {
    perror("Erreur de creation de la clé \n");
    exit(1);
  }

  if ((msgid = msgget(cle, IPC_CREAT | IPC_EXCL | 0750)) == -1)
    perror("Pb msgid");

  printf("MSGID :  %d\n", msgid);
}

int getShmid(){
  return shmid;
}

int getMsgid(){
  return msgid;
}


void printShmem(int shmid){

  Docks* shmem = (Docks *)shmat(getShmid(), NULL, 0);
  printf("---------\n");
  printf("Appercu dock :\n");
  printf("Trucks:\n");
  for (int i=0; i<NB_OF_TRUCKS; i++){
    printf("[%d/%c]",shmem->trucksSharedDock.trs[i].id,shmem->trucksSharedDock.trs[i].dest);
  }
  printf("\n");

  printf("Trucks container:\n");
  for (int i=0; i<NB_CONTAINER_TRUCK; i++){
    printf("[%d/%c]",shmem->trucksSharedDock.cont[i].id,shmem->trucksSharedDock.cont[i].dest);
  }
  printf("\n");

  printf("---\n");
  printf("Boat:\n");
  for (int i=0; i<NB_OF_BOATS; i++){
    printf("[%d/%c]",shmem->boatSharedDock.trs[i].id,shmem->boatSharedDock.trs[i].dest);
  }
  printf("\n");

  printf("Boat container:\n");
  for (int i=0; i<NB_CONTAINER_BOAT; i++){
    printf("[%d/%c]",shmem->boatSharedDock.cont[i].id,shmem->boatSharedDock.cont[i].dest);
  }
  printf("\n");

  printf("---\n");
  printf("Train:\n");
  for (int i=0; i<NB_OF_TRAINS; i++){
    printf("[%d/%c]",shmem->trainSharedDock.trs[i].id,shmem->trainSharedDock.trs[i].dest);
  }
  printf("\n");

  printf("Train container:\n");
  for (int i=0; i<NB_CONTAINER_TRAIN; i++){
    printf("[%d/%c]",shmem->trainSharedDock.cont[i].id,shmem->trainSharedDock.cont[i].dest);
  }
  printf("\n\n");

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
