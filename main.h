#include "transport.h"

//To know how much container and free place per destination there is
typedef struct ContainerDisposition{
  int nbExcessCont;//Can be negative, it means there are more free place
  char *destinations;
}ContainerDisposition;

void genCrane();//generate the crane (portique)
void genTransport();//generate the transports
void genTerrain(); //Not sur if usefull but might come handy

void main();
