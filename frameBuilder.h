#ifndef FRAMEBUILDER_H_INCLUDED
#define FRAMEBUILDER_H_INCLUDED
#include "EthernetInterface.h"
#include <vector>

int printDebug(int debug);
int frameBuilder(TCPSocketConnection *sock, char *nameTab, unsigned short *adresseTab);

#endif // FRAMEBUILDER_H_INCLUDED