#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define TRAIN 1
#define TRUCK 2
#define BOAT 3
#define FULL 4

typedef struct Dtrains{
  int cont[10];
  int trs[2];
}Dtrains;

typedef struct Dtrucks{
  int cont[10];
  int trs[10];
}Dtrucks;

typedef struct Dboat{
  int cont[6];
  int trs[2];
}Dboat;

typedef struct Docks{
  Dtrains trains;
  Dtrucks trucks;
  Dboat boat;
}Docks;
