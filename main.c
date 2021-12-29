#include "main.h"
#include "transportGeneration.h"
#include "transport.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>




void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

int main(){
  //create the shared memory struct
  shmemInit();
  Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);
  memset(docks, -1, sizeof(Docks));


  printShmem(getShmid());

  genCrane();
  genTerrain();
  genTransport(docks);
  return 0;
}

void genCrane(){


}

void genTerrain(){
  printf("Terrain generated with TID=%d\n",2);
}



//Return the biggest amount of missing container or missing places on dock(r[0]) and its corresponding destination(r[1])
int * getDockInequality(int containerDispositions[26]) {

  static int  r[2];

  int max = 0;
  int caseNb = -1;

  for(int i = 0 ; i < sizeof(int) ; i++){
    if(abs(containerDispositions[i]) > max){
      max = abs(containerDispositions[i]);
      caseNb = i;
    }
  }

  if(caseNb == -1){
    return NULL;
  }

  r[0] = containerDispositions[caseNb];
  r[1] = caseNb;
  return r;
}



void genTransport(Docks* docks){
  printf("Generating transport methods...\n");

  srand(time(NULL));

  //TO DO : make a malloc for the threads
  pthread_t thread[100];
  int i = 0;

  char destinations[] = {'A', 'B'};
  int incrementingId = 0, incrementingContainerId;
  //To know how much container per destination there is. Can also be NEGATIVE, in this case this is the number of free place on the vehicles
  //The case of the array give the destination(26 letters of the alphabet)
  int containerDispositions[26];

  //===Generate vehicles
  //Initialize shared memory
  for(int i = 0 ; i<NB_OF_TRUCKS ; i++)
      docks->trucksSharedDock.trs[i] = -1;
  for(int i = 0 ; i<NB_OF_TRAINS ; i++)
    docks->trainSharedDock.trs[i] = -1;
  for(int i = 0 ; i<NB_OF_BOATS ; i++)
      docks->boatSharedDock.trs[i] = -1;
  
  
  //Generate on the docks half empty and half filled vehicles, randomly
  int totalNumberOfPlacesOnDocks = NB_CONTAINER_TRUCK + NB_CONTAINER_TRAIN + NB_CONTAINER_BOAT;
  int currentDockPlacesTaken = 0;
  int randomPercentage = 0;
  int randomDestinationNo = 0;

  double trainProbability = 100*3/23;
  double truckProbability = 100*15/23;
  double boatProbability = 100*5/23;

  printf("Prob : %f, %f, %f\n" , trainProbability, truckProbability, boatProbability);

  int nbOfContainerPerTruck = NB_CONTAINER_TRUCK/NB_OF_TRUCKS;
  int nbOfContainerPerTrain = NB_CONTAINER_TRAIN/NB_OF_TRAINS;
  int nbOfContainerPerBoat = NB_CONTAINER_BOAT/NB_OF_BOATS;

  int nbOfVehiclesGenerated[3] = {0,0,0};//In the order : Truck, Train and Boat

  srand(time(NULL));

  container emptyTruckContArray[1];
  emptyTruckContArray[0].id = -1;
  
  container emptyTrainContArray[5];
  for(int i = 0 ; i<5 ; i++){
    emptyTrainContArray[i].id = -1;
  }

  container emptyBoatContArray[3];
  for(int i = 0 ; i<3 ; i++){
    emptyBoatContArray[i].id = -1;
  }


  //For detached threads
  pthread_attr_t thread_attr;
  if (pthread_attr_init (&thread_attr) != 0) {
    fprintf (stderr, "pthread_attr_init error");
    exit (1);
  }
  if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
    fprintf (stderr, "pthread_attr_setdetachstate error");
    exit (1);
  }

  printf("ttl number palce on dock : %d \n", totalNumberOfPlacesOnDocks);

  //===We generate half dock of empty vehicles
  while(currentDockPlacesTaken < totalNumberOfPlacesOnDocks/2){

    randomPercentage = rand()%100 +1;

    transport* transportToGenerate = malloc(sizeof(transport));
    sleep(1);
     randomDestinationNo = rand() % (sizeof(int));
    transportToGenerate->dest = destinations[randomDestinationNo];
    transportToGenerate->shmid = getShmid();
    transportToGenerate->id = incrementingId;

    //Generate randomly a vehicle with probability depending on the number of its container
    if(randomPercentage < trainProbability){
      //Train
      transportToGenerate->type = 't';
      transportToGenerate->contArray = emptyTrainContArray;
      nbOfVehiclesGenerated[1]++;
      currentDockPlacesTaken+=nbOfContainerPerTrain;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerTrain;

    }else if(randomPercentage < trainProbability + truckProbability && randomPercentage > trainProbability){
      //Truck
      transportToGenerate->type = 'T';
      transportToGenerate->contArray = emptyTruckContArray;
      nbOfVehiclesGenerated[0]++;
      currentDockPlacesTaken+=nbOfContainerPerTruck;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerTruck;

    }else{
      //Boat
      transportToGenerate->type = 'b';
      transportToGenerate->contArray = emptyBoatContArray;
      nbOfVehiclesGenerated[2]++;
      currentDockPlacesTaken+=nbOfContainerPerBoat;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerBoat;
 
    }
    
   if (pthread_create(thread+i, &thread_attr,(void *) transportFunc, transportToGenerate) != 0)
      perror("Erreur Creation thread");
    i++;
    incrementingId++;
    printf("coucou2\n");
    
    
  }
  
pause();

  int *inequality;
  //===We count the inequalities on dock and generate vehicles accordingly (mostly filled ones)
  while(currentDockPlacesTaken < totalNumberOfPlacesOnDocks){

    if(getDockInequality(containerDispositions) != NULL){
      inequality = getDockInequality(containerDispositions);
    }else{
      inequality[0] = 0;
    }
    
    transport* transportToGenerate = malloc(sizeof(transport));
    if(inequality[0] > 0){
    
    transportToGenerate->contArray = NULL;
    }else{

    }

    if( NB_OF_TRAINS < nbOfVehiclesGenerated[1]){
      //Trains
    }else if(NB_OF_BOATS < nbOfVehiclesGenerated[2]){
      //Boat
    }else{
      //Truck
    }


    //Generate trucks
    /*pthread_t thread[20];

    for(int i=0;i<20;i++){
      transport* t = malloc(sizeof(transport));
      t->shmem = docks;
      t->type = 'T';
      t->id = i;
      pthread_create(thread+i,0,(void *)transportFunc, t);
    }

    for(int i=0;i<20;i++){
      pthread_join(thread[i],NULL);
    }*/

  }

  //Generate randomly a few others vehicles


  //===Infinite loop
  //If a vehicle leave, transport generator is waken up (signal or monitor?)
  //Generate 0, 1 or 2 vehicles of the same type with equiprobability

}