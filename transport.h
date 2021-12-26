

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
  char dest;
  container* contArray;
  int pos;
  int shmid;
}transport;


void transportFunc(transport *t);//function to associate one function to each type

void boat(transport* t);//main function for the boat
void train(transport *t);//Idem
void truck(transport* t);//Idem
