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
  Docks* shmem;
  int id;
  int container;
}Crane;
#endif