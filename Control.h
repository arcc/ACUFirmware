#ifndef Control_h
#define Control_h
/******************************************************************************
 * Control.h - Library for controlling the LoFASM ARX.
 * Created by Anthony Ford, 2012
 *
 *
 *****************************************************************************/


#include "Arduino.h"

void Blink();

void Attens(int level);

int ReadAttens();

void WriteFEE(int channel, bool power);
bool ReadFEE(int channel);

void Filter(int filter);
int ReadFilter(void);

char Checksum(char payload[]);

void BuildPayload(char payload[]);
bool ProcessPayload(char payload[]);

void PinSetup(void);

#endif
