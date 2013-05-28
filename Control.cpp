/******************************************************************************
 * Control.cpp - Library for controlling the LoFASM ARX.
 * Created by Anthony Ford, 2012
 *
 *
 *****************************************************************************/

#include "Arduino.h"
#include <EEPROM.h>


int EEPROM_ADDR = 0;
int EEPROM_SIZE = 1024;
int FLASH_SIZE = 7;

int EEPROM_FEE_OFFSET[4] = {0,1,2,3};
int EEPROM_FILTER_OFFSET = 4;
int EEPROM_ATTEN_OFFSET[2] = {5,6};

int LED_SIGNAL = 13;

int F0 = 7;
int F1 = 6;

int FEE[4] = {2, 3, 4, 5};
//int FEE[4] = {5, 4, 3, 2};

int G0[4] = {8, 9, 10, 11};
int G1[4] = {A0, A1, A2, A3};

int ROACH_POWER = 12;
int ROACH_ON_DELAY = 500;
int ROACH_RESET_DELAY = 3000;

void Blink()
{
    digitalWrite(LED_SIGNAL, HIGH);
    delay(50);
    digitalWrite(LED_SIGNAL, LOW);
    delay(50);
    digitalWrite(LED_SIGNAL, HIGH);
    delay(50);
    digitalWrite(LED_SIGNAL, LOW);
    delay(50);
    digitalWrite(LED_SIGNAL, HIGH);
    delay(50);
    digitalWrite(LED_SIGNAL, LOW);
    delay(50);
}

bool WriteAtten(int atten, int levels)
{

    if (atten==0)
    {
        for (int i=0;i<4;i++)
        {
            digitalWrite(G0[i],bitRead(levels,i));
        }
        return true;
    }
    else if (atten==1)
    {
        for (int i=0;i<4;i++)
        {
            digitalWrite(G1[i],bitRead(levels,i));
        }
        return true;
    }   
    else
        return false;
}
int ReadAtten(int atten)
{
    int out = -1;
    if(atten==0)
    {
        out = 0;
        for (int i=0;i<4;i++)
        {
            bitWrite(out, i, digitalRead(G0[i]));
        }
    }
    else if (atten==1)
    {
        out = 0;
        for (int i=0;i<4;i++)
        {
            bitWrite(out, i, digitalRead(G1[i]));
        }
    }
    return out;
}

void WriteFEE(int channel, bool power)
{
    digitalWrite(FEE[channel], power);
}
bool ReadFEE(int channel)
{
    bool state = digitalRead(FEE[channel]);
    return state;
}

void WriteFilter(int filter)
{
    digitalWrite(F0, bitRead(filter,0));
    digitalWrite(F1, bitRead(filter,1));
}
int ReadFilter()
{
    int filter = 0;
    bitWrite(filter, 0, digitalRead(F0));
    bitWrite(filter, 1, digitalRead(F1));

    return filter;
}

bool WriteEEPROMAddr(int addr)
{
    if (addr > (int)(EEPROM_SIZE/FLASH_SIZE)-1)
        return false;
    else
        EEPROM.write(EEPROM_ADDR,(addr*FLASH_SIZE)+1);
}
int ReadEEPROMAddr()
{
    return (int)EEPROM.read(EEPROM_ADDR);
}

int FlashAddr()
{
    return ReadEEPROMAddr()*FLASH_SIZE;
}

void ReadFlash()
{
    //Reading FEE's
    for(int i=0;i<4;i++)
        WriteFEE(i,EEPROM.read(FlashAddr()+EEPROM_FEE_OFFSET[i]));

    WriteFilter(EEPROM.read(FlashAddr()+EEPROM_FILTER_OFFSET));

    //Write Attens
    WriteAtten(0,EEPROM.read(FlashAddr()+EEPROM_ATTEN_OFFSET[0]));
    WriteAtten(1,EEPROM.read(FlashAddr()+EEPROM_ATTEN_OFFSET[1]));
}

void WriteFlash()
{
    //Write FEE's
    for(int i=0;i<4;i++)
        EEPROM.write(FlashAddr()+EEPROM_FEE_OFFSET[i],ReadFEE(i));

    EEPROM.write(FlashAddr()+EEPROM_FILTER_OFFSET,ReadFilter());

    //Write Attens
    EEPROM.write(FlashAddr()+EEPROM_ATTEN_OFFSET[0],ReadAtten(0));
    EEPROM.write(FlashAddr()+EEPROM_ATTEN_OFFSET[1],ReadAtten(1));
}

void WriteRoach(int state)
{
    //Set ROACH_POWER high for a few seconds
    digitalWrite(ROACH_POWER, HIGH);
    if (state)//inverted so a 1 state means turning on ROACH
        delay(ROACH_ON_DELAY);
    else
        delay(ROACH_RESET_DELAY);
    digitalWrite(ROACH_POWER, LOW);
}
    

void PinSetup()
{
    pinMode(LED_SIGNAL, OUTPUT);
    digitalWrite(LED_SIGNAL, LOW);
    pinMode(F0, OUTPUT);
    pinMode(F1, OUTPUT);
    pinMode(ROACH_POWER, OUTPUT);
    for (int i=0;i<4;i++)
    {
        pinMode(G0[i], OUTPUT);
        pinMode(G1[i], OUTPUT);
        pinMode(FEE[i], OUTPUT);
    }
    
    //Defaults
    //WriteFEE(0,true);
    //WriteFEE(1,true);
    //WriteFEE(2,true);
    //WriteFEE(3,true);

    //WriteAtten(0,0);
    //WriteAtten(1,0);

    //WriteFilter(0);

    ReadFlash();
    
}

