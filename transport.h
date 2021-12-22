

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "container.h"
#include "docks.h"

//Structure to define transportation method
typedef struct transport{
  char type;
  int id;
  char* dest;
  container* contArray;
  int pos;
  Docks *shmem;
}transport;


typedef struct trainAndCommunication{
  transport train;
  Dtrains *sharedDock;
}trainAndCommunication;

void transportFunc(transport *t);//function to associate one function to each type

//Connect the transports to the message queue and shared memory
void boatArrival(transport *t);
void trainArrival(trainAndCommunication *t);
void truckArrival(transport *t);

void boat(transport* t);//main function for the boat
void train(trainAndCommunication *t);//Idem
void truck(transport* t);//Idem
