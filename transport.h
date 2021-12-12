#ifndef CLASS_H
#define CLASS_H


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

void trsFunc(transport t);//funtion to associate one function to each type
void boat(transport t);//main function for the boat
void train(transport t);//Idem
void truck(transport t);//Idem




#endif