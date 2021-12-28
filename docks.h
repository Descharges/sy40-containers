#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include "container.h"

#define TRAIN 1
#define TRUCK 2
#define BOAT 3
#define FULL 4

#define NB_OF_TRUCKS 10
#define NB_OF_TRAINS 2
#define NB_OF_BOATS 2

#define NB_CONTAINER_TRUCK 10
#define NB_CONTAINER_TRAIN 10
#define NB_CONTAINER_BOAT 6

typedef struct Dtrucks{
  container cont[NB_CONTAINER_TRUCK];
  int trs[NB_OF_TRUCKS];
}Dtrucks;

typedef struct Dtrains{
  container cont[NB_CONTAINER_TRAIN];
  int trs[NB_OF_TRAINS];
}Dtrains;

typedef struct Dboats{
  container cont[NB_CONTAINER_BOAT];
  int trs[NB_OF_BOATS];
}Dboats;

typedef struct Docks{
  Dtrains trainSharedDock;
  Dtrucks trucksSharedDock;
  Dboats boatSharedDock;
}Docks;

void shmemInit();

int getShmid();

void printShmem(int shmid);

void lock(int arg);
void unlock(int arg);
