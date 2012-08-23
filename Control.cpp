/******************************************************************************
 * Control.cpp - Library for controlling the LoFASM ARX.
 * Created by Anthony Ford, 2012
 *
 *
 *****************************************************************************/

#include "Arduino.h"
#include <EEPROM.h>


int EEPROM_ADDR = 0;

int LED_SIGNAL = 13;

int F0 = 6;
int F1 = 7;

//int FEE[4] = {2, 3, 4, 5};
int FEE[4] = {5, 4, 3, 2};

int G0[4] = {8, 9, 10, 11};
int G1[4] = {A0, A1, A2, A3};

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

void PinSetup()
{
    pinMode(LED_SIGNAL, OUTPUT);
    digitalWrite(LED_SIGNAL, LOW);
    pinMode(F0, OUTPUT);
    digitalWrite(F0, LOW);
    pinMode(F1, OUTPUT);
    digitalWrite(F1, LOW);
    for (int i=0;i<4;i++)
    {
        pinMode(G0[i], OUTPUT);
        digitalWrite(G0[i], HIGH);
        pinMode(G1[i], OUTPUT);
        digitalWrite(G1[i], HIGH);
        pinMode(FEE[i], OUTPUT);
        digitalWrite(FEE[i], LOW);
    }
}

bool WriteEEPROMAddr(int addr)
{
    if (addr > 254)
        return false;
    else
        EEPROM.write(EEPROM_ADDR,addr);
}
int ReadEEPROMAddr()
{
    return (int)EEPROM.read(EEPROM_ADDR);
}
