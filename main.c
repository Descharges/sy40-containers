#include "main.h"
#include "transport.h"
#include "crane.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define NUMBER_OF_DESTINATION 3 //Simply change this to increase the number of destinations (max 26)
int incrementingId = 0, incrementingContainerId = 0;



void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

extern int end;
extern pthread_cond_t nextCrane;


int main(){
  //create the shared memory struct
  shmemInit();
  msgQInit();
  Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);
  
  for(int i = 0 ; i<NB_OF_TRUCKS ; i++){
    docks->trucksSharedDock.trs[i].id = -1;
    docks->trucksSharedDock.trs[i].dest = 0;
  }
      
  for(int i = 0 ; i<NB_OF_TRAINS ; i++){
    docks->trainSharedDock.trs[i].id = -1;
    docks->trainSharedDock.trs[i].dest = 0;
  }
    
  for(int i = 0 ; i<NB_OF_BOATS ; i++){
    docks->boatSharedDock.trs[i].id = -1;
    docks->boatSharedDock.trs[i].dest = 0;
  }
      
  
  for(int i = 0 ; i<10 ; i++){
    docks->trucksSharedDock.cont[i].id = -1;
    docks->trucksSharedDock.cont[i].dest = 0;
  }
      
  for(int i = 0 ; i<10 ; i++){
    docks->trainSharedDock.cont[i].id = -1;
    docks->trainSharedDock.cont[i].dest = 0;
  } 
    
  for(int i = 0 ; i<6; i++){
    docks->boatSharedDock.cont[i].id = -1;
    docks->boatSharedDock.cont[i].dest = 0;
  }
      
  
  printf("[CONTROL] Shared memory allocated\n");

  genInitialTransport(docks);
  //genTransport
  pthread_t thread;
  if (pthread_create(&thread, 0,(void *) genTransport, NULL) != 0)
	        perror("Erreur Creation thread");

  //For testing purposes only
  printf("[CONTROL] Transports threads created\n");

  pthread_t crane1;
  Crane* c1 = malloc(sizeof(Crane));
  c1->id = 0;
  c1->shmid = getShmid();
  c1->genTransport = thread;
  pthread_create(&crane1, 0,(void*)craneFunc, c1);

  pthread_t crane2;
  Crane* c2 = malloc(sizeof(Crane));
  c2->id = 1;
  c2->shmid = getShmid();
  c2->genTransport = thread;
  pthread_create(&crane2, 0,(void*)craneFunc, c2);
  printf("[CONTROL] Crane thread created\n");
   



    

  pthread_join(crane1,NULL);
  pthread_join(crane1,NULL);

  free(c1);
  free(c2);




  msgctl(getMsgid(), IPC_RMID, NULL);
  shmctl(getShmid(), IPC_RMID, NULL);

  return 0;
}


//Return the biggest amount of missing container or missing places on dock(r[0]) and its corresponding destination(r[1])
int * getDockInequality(int containerDispositions[26]) {

  static int  r[2];
  
  int min = 0;
  int caseNb = -1;
  
  for(int i = 0 ; i < NUMBER_OF_DESTINATION ; i++){
    if(containerDispositions[i] < min){
      min = containerDispositions[i];
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

//Ask for a char, give the corresponding int
int getNoDestination(char* destinations, char desiredDestination){
  
  int noDestination;

  for(int i = 0 ; i<NUMBER_OF_DESTINATION ; i++){
    if(desiredDestination == destinations[i])
      noDestination = i;
  }
  
  return noDestination;
}



void genInitialTransport(Docks* docks){
  printf("Generating transport methods...\n");

  srand(time(NULL));


  pthread_t thread[50];

  char destinations[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R','S','T','U','V','W','X','Y','Z'};
  //To know how much container per destination there is. Can also be NEGATIVE, in this case this is the number of free place on the vehicles
  //The case of the array give the destination(26 letters of the alphabet)
  int containerDispositions[26];
  for(int i = 0 ; i<26 ; i++){
    containerDispositions[i] = 0;
  }

  //===Generate vehicles
  
  
  //We will generate on the docks half empty and half filled vehicles
  int currentDockPlacesTaken = 0;
  int randomDestinationNo = 0;
  
  int nbOfContainerPerTruck = NB_CONTAINER_TRUCK/NB_OF_TRUCKS;
  int nbOfContainerPerTrain = NB_CONTAINER_TRAIN/NB_OF_TRAINS;
  int nbOfContainerPerBoat = NB_CONTAINER_BOAT/NB_OF_BOATS;

  int nbOfVehiclesGenerated[3] = {0,0,0};//In the order : Truck, Train and Boat

  srand(time(NULL));

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




  //===We generate half dock of empty vehicles
  for(int i = 0 ; i<3 ; i++){


    transport* transportToGenerate = malloc(sizeof(transport));
    randomDestinationNo = rand() % NUMBER_OF_DESTINATION;

    transportToGenerate->dest = destinations[randomDestinationNo];
    transportToGenerate->shmid = getShmid();
    transportToGenerate->id = incrementingId;

    if(i == 1){
       //Boat
      transportToGenerate->type = 'b';
      container *emptyBoatContArray = malloc(sizeof(container)*3);
      for(int i = 0 ; i<3 ; i++){
        emptyBoatContArray[i].id = -1;
        emptyBoatContArray[i].dest = 0;
      }

      transportToGenerate->contArray = emptyBoatContArray;
      nbOfVehiclesGenerated[2]++;
      currentDockPlacesTaken+=nbOfContainerPerBoat;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerBoat;
   
    }else{
     //Train

      transportToGenerate->type = 't';
      container *emptyTrainContArray = malloc(sizeof(container)*5);
      for(int i = 0 ; i<5 ; i++){
        emptyTrainContArray[i].id = -1;
        emptyTrainContArray[i].dest = 0;
      }
      transportToGenerate->contArray = emptyTrainContArray;
      nbOfVehiclesGenerated[1]++;
      currentDockPlacesTaken+=nbOfContainerPerTrain;
      containerDispositions[randomDestinationNo]-=nbOfContainerPerTrain;
 
    }
    
   if (pthread_create(thread+i, &thread_attr,(void *) transportFunc, transportToGenerate) != 0)

      perror("Erreur Creation thread");
    incrementingId++;
    
    
  }
  
  
  sleep(1);
  int *inequality = malloc(2*sizeof(int));
  


  //===We count the inequalities(is there more container of free places?) on dock and generate filled vehicles accordingly 
  for(int i = 0 ; i<11 ; i++){

    transport* transportToGenerate = malloc(sizeof(transport));
    transportToGenerate->shmid = getShmid();
    transportToGenerate->id = incrementingId;

    //Get the deficit container/free place
    if(getDockInequality(containerDispositions) != NULL){
      inequality = getDockInequality(containerDispositions);
    }else{
      inequality[1] = -1;
    }
    
    //Give a random destination which is not the one of the containers
    randomDestinationNo=inequality[1];
    while(randomDestinationNo == inequality[1]){
      randomDestinationNo = rand() % NUMBER_OF_DESTINATION;
    }
    transportToGenerate->dest = destinations[randomDestinationNo];


    if(i == 1){
      //Boat
      transportToGenerate->type = 'b';
      container *filledBoatContArray = malloc(sizeof(container)*3);
      //Give container to the boat according to the needs of the dock
      for(int j = 0 ; j<3 ; j++){
        filledBoatContArray[j].id = incrementingContainerId;
        incrementingContainerId++;
        filledBoatContArray[j].dest = destinations[inequality[1]];
      }

      transportToGenerate->contArray = filledBoatContArray;
      nbOfVehiclesGenerated[2]++;
      currentDockPlacesTaken+=nbOfContainerPerBoat;
      containerDispositions[inequality[1]]+=nbOfContainerPerBoat;
   
    }else{
      //Truck
      transportToGenerate->type = 'T';
      container *filledTruckContArray = malloc(sizeof(container)*1);
      filledTruckContArray[0].id = incrementingContainerId;
      incrementingContainerId++;
      filledTruckContArray[0].dest = destinations[inequality[1]];

      transportToGenerate->contArray = filledTruckContArray;
      nbOfVehiclesGenerated[0]++;
      currentDockPlacesTaken+=nbOfContainerPerTruck;
      containerDispositions[inequality[1]]+=nbOfContainerPerTruck;
    }
    
    
    
    if (pthread_create(thread+i, &thread_attr,(void *) transportFunc, transportToGenerate) != 0)
      perror("Erreur Creation thread");
    incrementingId++;

  }
  
  sleep(1);


}




void genTransport(){

  struct sigaction intSigaction;
  intSigaction.sa_handler = intHandler;
  sigemptyset (&intSigaction.sa_mask);
  intSigaction.sa_flags = 0;
  sigaction(SIGINT, &intSigaction, NULL);
  
  sleep(2);
  char destinations[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R','S','T','U','V','W','X','Y','Z'};
  //TO DO : malloc the pthread_t
  pthread_t thread[50];

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

  int randomDestinationNo = 0, randomNbOfVehicle = 0;;

  int *inequality;
  char type;
  
  struct sigaction unpauseSigaction;
  unpauseSigaction.sa_handler = sigHandler;
  sigemptyset (&unpauseSigaction.sa_mask);
  unpauseSigaction.sa_flags = 0;
  sigaction(SIGUSR1, &unpauseSigaction, NULL);

  Docks* docks = (Docks *)shmat(getShmid(), NULL, 0);

  struct msqid_ds buf;

  trsDest infoAnswer;

  int numMsg = -1;

  
  
  
 while(1){
  pause();

  if (end==1) {
    //free ici
    pthread_exit(0);
    pthread_cond_signal(&nextCrane);
  }


  //=== Get msg from crane to know which vehicle is gone
  //Get ALL waiting msg
  
  if(msgctl(getMsgid(), IPC_STAT, &buf) == -1){
    perror("msgctl error while getting number of messages.");
    exit(1);
  }
    
  numMsg = buf.msg_qnum;

  //We generate as much vehicule as messages
  while(numMsg !=0){
  
  if(msgctl(getMsgid(), IPC_STAT, &buf) == -1){
    perror("msgctl error while getting number of messages.");
  }
    
  numMsg = buf.msg_qnum;

  if ((msgrcv(getMsgid(), &infoAnswer, sizeof(trsDest), 1, 0)) == -1) {
    perror("Erreur de lecture requete P2\n");
  }

  printf("[GENERATOR]Generate new '%c'\n", infoAnswer.vehicleType);
  fflush(stdout);


  type = infoAnswer.vehicleType;
  
  container *filledTruckContArray = malloc(sizeof(container)*1);
  container *filledTrainContArray = malloc(sizeof(container)*3);
  container *filledBoatContArray = malloc(sizeof(container)*3);
  
  //=== Get dock shmem inequalities(difference between container and free places)
    
    //Same functionning as genInitialTransport
    int containerDispositions[26];
    for(int i = 0 ; i<26 ; i++)
      containerDispositions[i] = 0;
     
    lock(FULL);
    for(int i = 0 ; i<NB_CONTAINER_TRUCK ; i++){
      if(docks->trucksSharedDock.cont[i].id == -1){
        containerDispositions[getNoDestination(destinations, docks->trucksSharedDock.trs[i].dest)]-=1;
      }else{
        containerDispositions[getNoDestination(destinations, docks->trucksSharedDock.cont[i].dest)] +=1;
       
      }
    }
    
   

    for(int i = 0 ; i<NB_CONTAINER_TRAIN ; i++){
      if(docks->trainSharedDock.cont[i].id == -1){
        containerDispositions[getNoDestination(destinations, docks->trainSharedDock.trs[i/5].dest)]-=1;
      }else{
        containerDispositions[getNoDestination(destinations, docks->trainSharedDock.cont[i].dest)] +=1;
      }
    }

    for(int i = 0 ; i<NB_CONTAINER_BOAT ; i++){
      if(docks->boatSharedDock.cont[i].id == -1){
        containerDispositions[getNoDestination(destinations, docks->boatSharedDock.trs[i/3].dest)]-=1;
      }else{
        containerDispositions[getNoDestination(destinations, docks->boatSharedDock.cont[i].dest)] +=1;
      }
    }
    unlock(FULL);
    //If negative : give number of container
    printf("[GENERATOR]Number of free places to A : %d, B : %d, C : %d\n", -containerDispositions[0], -containerDispositions[1], -containerDispositions[2]);
    
    inequality = getDockInequality(containerDispositions);

  
  //=== Generate vehicle according to msg and inequality
    randomNbOfVehicle = 1;
    for(int h = 0 ; h < randomNbOfVehicle ; h++){
      transport* transportToGenerate = malloc(sizeof(transport));
      
      //The transport should not have the same destinations as its containers
      do{
        randomDestinationNo = rand() % NUMBER_OF_DESTINATION;
      }while(inequality != NULL && inequality[1] == randomDestinationNo);

      transportToGenerate->dest = destinations[randomDestinationNo];
      transportToGenerate->shmid = getShmid();
      transportToGenerate->id = incrementingId;
      
      if(type == 'b'){
        //Boat
        transportToGenerate->type = 'b';
        
        for(int j = 0 ; j<3 ; j++){

          filledBoatContArray[j].id = incrementingContainerId;
          incrementingContainerId++;

          //If there is no need of container on docks
          if(inequality == NULL){
          //Generate free places
          filledBoatContArray[j].id = -1;
          filledBoatContArray[j].dest = -1; 
          }else{
            //Generate containers
            filledBoatContArray[j].dest = destinations[inequality[1]]; 
          }
        }

        transportToGenerate->contArray = filledBoatContArray;
      }else if(type == 'T'){
        //Truck
        transportToGenerate->type = 'T';
        
        filledTruckContArray[0].id = incrementingContainerId;
        incrementingContainerId++;
        if(inequality == NULL){
          //Generate free places
          filledTruckContArray[0].id = -1;
          filledTruckContArray[0].dest = 0;  
          }else{
            //Generate containers
            filledTruckContArray[0].dest = destinations[inequality[1]];
          }
        

        transportToGenerate->contArray = filledTruckContArray;
      }else{
        //Boat
        transportToGenerate->type = 't';
        
        //Give container to the boat according to the needs of the dock
        for(int j = 0 ; j<5 ; j++){
          filledTrainContArray[j].id = incrementingContainerId;
          incrementingContainerId++;

          if(inequality == NULL){
          //Generate free places
          filledTrainContArray[j].id = -1;
          filledTrainContArray[j].dest = 0;
          }else{
            //Generate containers
            filledTrainContArray[j].dest = destinations[inequality[1]];
          }
          
        }

        transportToGenerate->contArray = filledTrainContArray;
      }


      if (pthread_create(thread, &thread_attr,(void *) transportFunc, transportToGenerate) != 0)
        perror("Erreur Creation thread");
      incrementingId++;

  
    }    
  }
 }
}