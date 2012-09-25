#ifndef Control_h
#define Control_h
/******************************************************************************
 * Control.h - Library for controlling the LoFASM ARX.
 * Created by Anthony Ford, 2012
 *
 *
 *****************************************************************************/


#include "Arduino.h"

extern int EEPROM_SIZE;
extern int FLASH_SIZE;

void Blink();

bool WriteAtten(int atten, int level);
int ReadAtten(int atten);

void WriteFEE(int channel, bool power);
bool ReadFEE(int channel);

void WriteFilter(int filter);
int ReadFilter(void);

bool WriteEEPROMAddr(int address);
int ReadEEPROMAddr();

bool FlashAddr();

void WriteFlash();
void ReadFlash();

void PinSetup(void);

#endif
