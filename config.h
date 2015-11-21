#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "mbed.h"
#include <string.h>
#include <vector>


short locoCount();
int loadFromFile(unsigned short *address);
int loadFromFile(char *names,unsigned short *address);
void loadConnectionInfos(char *connectionTab);

extern "C" void mbed_reset();

#endif // CONFIG_H_INCLUDED