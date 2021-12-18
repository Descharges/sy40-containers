#include "transport.h"
#include <pthread.h>

//===Mutex to handle the access to the docks
//Train
pthread_mutex_t trainMutex = PTHREAD_MUTEX_INITIALIZER;
//Truck
pthread_mutex_t truckMutex = PTHREAD_MUTEX_INITIALIZER, posMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t truckQueue = PTHREAD_COND_INITIALIZER, truckAdv= PTHREAD_COND_INITIALIZER;
pthread_mutex_t advMutex = PTHREAD_MUTEX_INITIALIZER;
char pos[10];

int nTrucks = 0;

void trsFunc(transport* t){
  switch (t->type){
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




//Place the train on the right position on the dock

//Connect the transports to the message queue and shared memory? Or should these function be removed?
void boatArrival(transport* t){}
void trainArrival(trainAndCommunication* t){
  printf("Train %d is arrived\n", t->train.id);
  //Connect the train shared memory and message queue

  //Update of the shared memory if needed

  //Give the content to the control tower/crane

  train(t);

}
void truckArrival(transport* t){}




void boat(transport *t){}


void train(trainAndCommunication *t){

  bool filledWithGoodDestination = true;//Should be set to FALSE at the beginning
  bool leave = false;
  bool isOnTopPosition = false;

  //Get the struct parameter of the function
  trainAndCommunication trainAndCom = *((trainAndCommunication*)(t));
  transport train = trainAndCom.train;
    

  //For TEST purpose
  if(train.id == 0){
      printf("Train %d top position occupied : %s\n", train.id, *(trainAndCom.topPositionOccupied) ? "true" : "false");
      
      isOnTopPosition = true;
      pthread_mutex_lock(&trainMutex);
        *(trainAndCom.topPositionOccupied) = true;
      pthread_mutex_unlock(&trainMutex);
  }else{
    printf("Train %d top position occupied: %s\n", train.id, *(trainAndCom.topPositionOccupied) ? "true" : "false");
    
    isOnTopPosition = false;
  }




  while( ! leave){
  //===The train wake up, we unlock the monitor

  //===Get the content of the message queue

  //===Remove or add container

    
  //===Check if the train should move
    pthread_mutex_lock(&trainMutex);
      if(*(trainAndCom.topPositionOccupied) == false){
        //move to the top position
        isOnTopPosition = true;
        *(trainAndCom.topPositionOccupied) = true;

      }
    pthread_mutex_unlock(&trainMutex);

    if(filledWithGoodDestination && isOnTopPosition){
      //Leave
      leave==true;
      pthread_mutex_lock(&trainMutex);
        *(trainAndCom.topPositionOccupied) = false;
      pthread_mutex_unlock(&trainMutex);

      printf("Train %d is leaving\n", train.id);
     break;
    }
    

  }



}



void truck(transport* t){
  //Wait for signal that a case is available for him
  pthread_mutex_lock(&truckMutex);
  if (nTrucks < 10){
    t->pos = 10-nTrucks;
  }else{
    //printf("[TRUCK ID=%d]Waiting to enter docks...\n",t->id);
    pthread_cond_wait(&truckQueue, &truckMutex);
    t->pos = 1;
  }
  nTrucks++;
  pthread_mutex_unlock(&truckMutex);

  //Change his value in the row
  pthread_mutex_lock(&posMutex);
  pos[(t->pos)-1] = t->id;
  pthread_mutex_unlock(&posMutex);

  //printf("[TRUCK ID=%d]The truck entered the dock at position %d\n",t->id,t->pos);

  while(t->pos != 10){

    pthread_mutex_lock(&advMutex);
    pthread_cond_wait(&truckAdv, &advMutex);
    pthread_mutex_unlock(&advMutex);

    t->pos++;
    pthread_mutex_lock(&posMutex);
    pos[(t->pos)-1] = t->id;
    pthread_mutex_unlock(&posMutex);
    if(t->pos == 2){
      pthread_cond_signal(&truckQueue);
    }
    //printf("[TRUCK ID=%d]The truck move forward to position %d\n",t->id,t->pos);
  }

  sleep(1); //Here goes the dicision to go or not

  //I should secure all thoses prints but :
  //1-Prints are read only
  //2-It's only debug and won't stay
  //3-I'm lazy

  printf("\e[1;1H\e[2J");
  printf("[TRUCK ID=%d]The truck is leaving...\n",t->id);
  printf("[TRUCK ID=%d]Number of trucks =%d\n",t->id,nTrucks);
  for(int i=0;i<10;i++){
    printf("-[%d]-",pos[i]);
  }
  printf("\n\n");


  pthread_mutex_lock(&truckMutex);
  nTrucks--;
  pthread_mutex_unlock(&truckMutex);

  //reset the array to avoid duplicates
  pthread_mutex_lock(&posMutex);
  memset(pos, 0, sizeof pos);
  pthread_mutex_unlock(&posMutex);
  pthread_cond_broadcast(&truckAdv);

  free(t);
  pthread_exit(NULL);


}
