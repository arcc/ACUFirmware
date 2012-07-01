/******************************************************************************
 * Control.cpp - Library for controlling the LoFASM ARX.
 * Created by Anthony Ford, 2012
 *
 *
 *****************************************************************************/

#include "Arduino.h"

int LED_SIGNAL = 13;

int F0 = 6;
int F1 = 7;

int FEE[4] = {2, 3, 4, 5};

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

void Attens(int levels)
{
    for (int i=0;i<4;i++)
    {
        digitalWrite(G0[i],bitRead(levels,i));
    }
    for (int i=0;i<4;i++)
    {
        digitalWrite(G1[i],bitRead(levels,i+4));
    }
}

int ReadAttens()
{
    int out;
    for (int i=0;i<4;i++)
    {
        bitWrite(out, i, digitalRead(G0[i]));
    }
    for (int i=0;i<4;i++)
    {
        bitWrite(out, i+4, digitalRead(G1[i]));
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
void Filter(int filter)
{
    digitalWrite(F0, bitRead(filter,0));
    digitalWrite(F1, bitRead(filter,1));
}

int ReadFilter()
{
    int filter;
    bitWrite(filter, 0, digitalRead(F0));
    bitWrite(filter, 1, digitalRead(F1));

    return filter;
}

char Checksum(char payload[])
{
    return payload[0] ^ payload[1] ^ payload[2];
}

void BuildPayload(char payload[])
{
    payload[0] = 0xff;
    payload[1] = char(ReadAttens());

    int fee_filter, filter;
    filter = ReadFilter();
    bitWrite(fee_filter, 4, bitRead(filter, 0));
    bitWrite(fee_filter, 5, bitRead(filter, 1));
    for (int i=0; i<4; i++)
        bitWrite(fee_filter, i, ReadFEE(i));

    payload[2] = fee_filter;
    payload[3] = Checksum(payload);
}

bool ProcessPayload(char payload[])
{
    if (Checksum(payload) == payload[3])
    { 
        int levels = int(payload[1]);
        int fee_filter = int(payload[2]);

        Attens(levels);
        int filter;
        bitWrite(filter, 0, bitRead(fee_filter, 4));
        bitWrite(filter, 1, bitRead(fee_filter, 5));
        Filter(filter);
        for (int i=0; i<4; i++)
            WriteFEE(i,bitRead(fee_filter, i));

        return true;
    }
    else
        return false;
}

void PinSetup()
{
    pinMode(LED_SIGNAL, OUTPUT);
    pinMode(F0, OUTPUT);
    pinMode(F1, OUTPUT);
    for (int i=0;i<4;i++)
    {
        pinMode(G0[i], OUTPUT);
        pinMode(G1[i], OUTPUT);
        pinMode(FEE[i], OUTPUT);
    }
}
