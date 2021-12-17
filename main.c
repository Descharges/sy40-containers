#include "main.h"
#include "transportGeneration.h";

#include <stdio.h>

void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

void main(){
  genCrane();
  genTerrain();
  genTransport();
}

void genCrane(){
  printf("Crane generated with TID=%d\n",1);
}

void genTerrain(){
  printf("Terrain generated with TID=%d\n",2);
}

void genTransport(){
  printf("Generating transport methods...");
  generateTrains();
  while(1){}
}
