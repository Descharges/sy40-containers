#include "transport.h"

void trsFunc(transport t){
  switch (t.type){
    case 'b'://boat
      boat(t);
      break;
    case 't'://train
      train(t);
      break;
    case 'T'://Truck (maybe find a better letter ? idk)
      truck(t);
      break;
  }
}

void boat(transport t){}
void train(transport t){}
void truck(transport t){}
