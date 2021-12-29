#include "main.h"
#include "transportGeneration.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

void main(){
  //create the shared memory struct
  Docks *docks;
  docks = init();

  genCrane();
  genTerrain();
  genTransport(docks);
}

void genCrane(){
  printf("Crane generated with TID=%d\n",1);
}

void genTerrain(){
  printf("Terrain generated with TID=%d\n",2);
}

//Return the biggest amount of missing container or missing places on dock(r[0]) and its corresponding destination(r[1])
int * getDockInequality(int containerDispositions[26]) {

  static int  r[2];

  int max = 0;
  int caseNb = -1;

  for(int i = 0 ; i < sizeof(containerDispositions)/sizeof(containerDispositions[0]) ; i++){
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
  int incrementingContainerId;
  //To know how much container per destination there is. Can also be NEGATIVE, in this case this is the number of free place on the vehicles
  //The case of the array give the destination
  int containerDispositions[26];

  //===Generate vehicles
  //Initialize shared memory
  for(int i = 0 ; i<NB_OF_TRUCKS ; i++)
      docks->truckDock->trs[i] = -1;
  for(int i = 0 ; i<NB_OF_TRAINS ; i++)
    docks->trainDock->trs[i] = -1;
  for(int i = 0 ; i<NB_OF_BOATS ; i++)
      docks->boatDock->trs[i] = -1;
  
  
  //Generate on the docks half empty and half filled vehicles, randomly
  int totalNumberOfPlacesOnDocks = NB_CONTAINER_TRUCK + NB_CONTAINER_TRAIN + NB_CONTAINER_BOAT;
  int currentDockPlacesTaken = 0;
  int randomPercentage = 0;
  int randomDestinationNo = 0;

  double trainProbability = 100*1/(NB_CONTAINER_TRAIN/NB_CONTAINER_TRUCK + NB_CONTAINER_TRAIN/NB_CONTAINER_BOAT + 1);
  double truckProbability = 100*trainProbability*NB_CONTAINER_TRAIN/NB_CONTAINER_TRUCK;
  double boatProbability = 100*trainProbability*NB_CONTAINER_TRAIN/NB_CONTAINER_BOAT;

  int nbOfContainerPerTruck = NB_CONTAINER_TRUCK/NB_OF_TRUCKS;
  int nbOfContainerPerTrain = NB_CONTAINER_TRAIN/NB_OF_TRAINS;
  int nbOfContainerPerBoat = NB_CONTAINER_BOAT/NB_OF_BOATS;

  int nbOfVehiclesGenerated[3] = {0,0,0};//In the order : Truck, Train and Boat

  srand(time(NULL));

  //===We generate half dock of empty vehicles
  while(currentDockPlacesTaken < totalNumberOfPlacesOnDocks/2){

    randomPercentage = rand()%100 +1;

    transport* transportToGenerate = malloc(sizeof(transport));
    transportToGenerate->contArray = NULL;
    randomDestinationNo = rand() % (sizeof(destinations)/sizeof(destinations[0]));
    transportToGenerate->dest = destinations[randomDestinationNo];

    //Generate randomly a vehicle with probability depending on the number of its container
    if(randomPercentage < trainProbability){
      //Train
      transportToGenerate->shmem.trainDock = docks->trainDock;
      transportToGenerate->type = 't';
      nbOfVehiclesGenerated[1]++;
      currentDockPlacesTaken+=nbOfContainerPerTrain;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerTrain;

    }else if(randomPercentage < trainProbability + truckProbability && randomPercentage > trainProbability){
      //Truck
      transportToGenerate->shmem.truckDock = docks->truckDock;
      transportToGenerate->type = 'T';
      nbOfVehiclesGenerated[0]++;
      currentDockPlacesTaken+=nbOfContainerPerTruck;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerTruck;

    }else{
      //Boat
      transportToGenerate->shmem.boatDock = docks->boatDock;
      transportToGenerate->type = 'b';
      nbOfVehiclesGenerated[2]++;
      currentDockPlacesTaken+=nbOfContainerPerBoat;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerBoat;

    }
    if (pthread_create(thread+i, 0,(void *) transportFunc, transportToGenerate) != 0)
      perror("Erreur Creation thread");
    i++;

  }
  



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