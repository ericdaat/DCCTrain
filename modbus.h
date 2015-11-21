#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED

#include "EthernetInterface.h"

extern "C" void mbed_reset();

void printBuffer(char *buffer);
void modbus_connect(TCPSocketConnection *sock);
int splitShort(unsigned short shortToSplit,char *part1,char *part2);
void modbus_read(short first_register,short nb_registers, TCPSocketConnection *sock,unsigned short* registerTab);
int mergeChar(char char1, char char2, short *mergedShort);

#define DHCP        atoi(connectionTab[0])
#define IPCARTE     connectionTab[1]
#define MASQUE      connectionTab[2]
#define PASSERELLE  connectionTab[3]
#define IPAUTOMATE  connectionTab[4]


#endif // MODBUS_H_INCLUDED