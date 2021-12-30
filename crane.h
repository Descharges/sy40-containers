#ifndef CRANE
#define CRANE 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "docks.h"

typedef struct Crane{
  int id;
  int shmid;
}Crane;

void pickAndPlace(char trs1, int x1, char trs2, int x2, Docks* shmem);
void* craneFunc(Crane* c);
#endif