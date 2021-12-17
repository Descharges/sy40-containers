

#include <stdbool.h>
#include "container.h"
#include "train.h"

//Structure to define transportation method
typedef struct transport{
  char type;
  int id;
  char* dest;
  container* contArray;
  int pos;
}transport;

void transportFunc(transport t);//funtion to associate one function to each type

//Connect the transports to the message queue and shared memory
void boatArrival(transport t);
void trainArrival(transport t);
void truckArrival(transport t);

void boat(transport t);//main function for the boat
void *train(void *t);//Idem
void truck(transport t);//Idem



