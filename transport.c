#include "transport.h"
#include "train.h"

//Mutex to handle the access to the docks
pthread_mutex_t truckMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t truckQueue = PTHREAD_COND_INITIALIZER;

int nTrucks = 0;

void trsFunc(transport* t){
  switch (t->type){
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


//Connect the transports to the message queue and shared memory
void boatArrival(transport* t){}
void trainArrival(transport* t){
  //Connect the train shared memory and message queue

  //Update of the shared memory if needed

  //Give the content to the control tower/crane

}
void truckArrival(transport* t){}



void boat(transport* t){}
void train(transport* t){

  bool filled = false;
  bool gone = false;
  trainArrival(t);


  while( ! gone){
    //The train wake up, we unlock the monitor

    //Get the content of the shared memory and message queue

    //Remove or add container

    //Check if the train should move

  }



}
void truck(transport* t){
  //Wait for signal that a case is available for him
  pthread_mutex_lock(&truckMutex);
  if (nTrucks < 10){
    t->pos = 10-nTrucks;
    nTrucks++;
  }else{
    printf("[TRUCK ID=%d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&truckQueue, &truckMutex);
  }
  pthread_mutex_unlock(&truckMutex);

  printf("[TRUCK ID=%d]The truck entered the dock at position %d\n",t->id,t->pos);

  sleep(3);

  printf("[TRUCK ID=%d]The truck is leaving...\n",t->id,t->pos);

  pthread_mutex_lock(&truckMutex);
  nTrucks--;
  pthread_cond_signal(&truckQueue);

  pthread_mutex_unlock(&truckMutex);
  free(t);
  pthread_exit(NULL);


}
