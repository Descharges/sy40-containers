#include "transport.h"
#include "train.h"

void transportFunc(transport t){
  switch (t.type){
    case 'b'://boat
      boat(t);
      break;
    case 't'://train
      train(&t);
      break;
    case 'T'://Truck (maybe find a better letter ? idk)
      truck(t);
      break;
  }
}



void boatArrival(transport t){}

void trainArrival(transport t){


  //Place the train on the right position on the dock

  //Connect the train shared memory and message queue
  
  //Update of the shared memory if needed

  //Give the content to the control tower/crane

}
void truckArrival(transport t){}



void boat(transport t){}
void *train(void *t){

  bool filled = false;
  bool gone = false;

  transport train = *((transport*)(t));

  printf("Train %d is here\n", train.id);

  trainArrival(train);


  while( ! gone){
    //The train wake up, we unlock the monitor

    //Get the content of the shared memory and message queue

    //Remove or add container

    //Check if the train should move

  }
  


}
void truck(transport t){}
