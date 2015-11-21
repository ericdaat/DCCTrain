#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include <vector>
#include "ressources.h"

int printDebug(int debug);
int binaryConversion(unsigned long long target);

void printBuffer(char *buffer);
void set_flags_NB_LOCO();

void LCD_Control();
void dccBinaryCoder();

#endif // MAIN_H_INCLUDED