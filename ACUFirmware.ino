/******************************************************************************
ARXTalk
-------

ARXTalk is an Arduino based firmware for controling the LoFASM
Analog Recievers.

Commands:
    

Dependencies:
    * [CmdMessenger](https://github.com/dreamcat4/CmdMessenger)
    * [Base64](https://github.com/adamvr/arduino-base64)
    * [Streaming](http://arduiniana.org/libraries/streaming/)
    
******************************************************************************/

// CmdMessenger library available from https://github.com/dreamcat4/CmdMessenger
#include <CmdMessenger.h>
#include <EEPROM.h>

// Base64 library available from https://github.com/adamvr/arduino-base64
/*#include <Base64.h>*/

// Streaming4 library available from http://arduiniana.org/libraries/streaming/
#include <Streaming.h>

#include "Control.h"

// Must not conflict / collide with our message payload data. Fine if we use
// base64 library above
char field_separator = ',';
char command_separator = ';';

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);


// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// We can define up to a default of 50 cmds total, including both directions
// (send + recieve) and including also the first 4 default command codes for the
// generic error handling.  If you run out of message slots, then just increase
// the value of MAXCALLBACKS in CmdMessenger.h

// Commands we send from the Arduino to be received on the PC
enum
{
  kCOMM_ERROR     = 000, // Lets Arduino report serial port comm error back to 
                        // the PC (only works for some comm errors)

  kACK            = 001, // Arduino acknowledges cmd was received

  kREADY          = 002, // After opening the comm port, send this cmd 02 from 
                        // PC to check arduino is ready

  kERR            = 003, // Arduino reports badly formatted cmd, or cmd not 
                        // recognise
  
  // We can define more 'send' commands, coming from the arduino.
  // For the above commands, we just call cmdMessenger.sendCmd() anywhere we
  // want in our Arduino program.

  kSEND_CMDS_END, // Must not delete this line
};

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function each entry in the list below.
// They start at the address kSEND_CMDS_END defined, above as 004

messengerCallbackFunction messengerCallbacks[] = 
{
  ready,            // 004
  fee_read,         // 005
  fee_write,        // 006
  filter_read,      // 007
  filter_write,     // 008
  atten_read,       // 009
  atten_write,      // 010
  eeprom_read,      // 011
  eeprom_write,     // 012
  flash_write,      // 013
  roach_power,      // 014
  
  NULL
};




// ------------------ C A L L B A C K  M E T H O D S -------------------------

void ready()
{
    cmdMessenger.sendCmd(kREADY,"READY");
    Blink();
}

void fee_read()
{
    char buf[2] = {'\0'};
    cmdMessenger.copyString(buf, 2);
    if(buf[0])
    {
        int chan = atoi(buf);
        if(chan < 4 && chan>=0)
        {
            char out [2] = {'\0'};
            itoa((int)ReadFEE(chan),out,10);
            cmdMessenger.sendCmd(kACK,out);
        }
        else
            cmdMessenger.sendCmd(kERR,"Filter selection is out of range");
    }
    Blink();
}

void fee_write()
{
    char buf[4] = {'\0'};
    cmdMessenger.copyString(buf, 4);
    if(buf[0])
    {
        char tmp[4] = {'\0'};
        tmp[0] = buf[0];
        int chan = atoi(tmp);
        if(chan < 4 && chan>=0)
        {
            if(buf[1] == '|')
            {
                tmp[0] = buf[2];
                int pwr = atoi(tmp);
                WriteFEE(chan,(bool)pwr);
                cmdMessenger.sendCmd(kACK,"FEE State Written");
            }
            else
                cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
        }
        else
            cmdMessenger.sendCmd(kERR,"FEE selection is out of range");
    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}        

void atten_read()
{
    char buf[2] = {'\0'};
    cmdMessenger.copyString(buf, 2);
    int atten = atoi(buf);
    if(buf[0])
    {
        if(atten < 2 && atten>=0)
        {
            char out [4] = {'\0'};
            itoa(ReadAtten(atten),out,10);
            cmdMessenger.sendCmd(kACK,out);
        }
        else
            cmdMessenger.sendCmd(kERR,"Atten selection is out of range");
    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}

void atten_write()
{
    char buf[5] = {'\0'};
    cmdMessenger.copyString(buf, 5);
    if(buf[0])
    {
        char tmp[4] = {'\0'};
        tmp[0] = buf[0];
        int chan = atoi(tmp);
        if(chan < 2 && chan>=0)
        {
            if(buf[1] == '|')
            {
                tmp[0] = buf[2];
                tmp[1] = buf[3];
                int lvl = atoi(tmp);
                WriteAtten(chan,lvl);
                cmdMessenger.sendCmd(kACK,"Atten State Written");
            }
            else
                cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
        }
        else
            cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}        

void filter_read()
{
    char out [4] = {'\0'};
    itoa(ReadFilter(),out,10);
    cmdMessenger.sendCmd(kACK,out);
    Blink();
}

void filter_write()
{
    char buf[2] = {'\0'};
    cmdMessenger.copyString(buf, 2);
    if(buf[0])
    {
        int chan = atoi(buf);
        if (chan < 3 && chan >= 0)
        {
            WriteFilter(chan);
            cmdMessenger.sendCmd(kACK,"Filter State Written");
        }
        else
            cmdMessenger.sendCmd(kERR,"Filter selection is out of range");

    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}        

void eeprom_read()
{
    char out [4] = {'\0'};
    itoa(ReadEEPROMAddr(),out,10);
    cmdMessenger.sendCmd(kACK,out);
    Blink();
}
void eeprom_write()
{
    char buf[6] = {'\0'};
    cmdMessenger.copyString(buf, 6);
    if(buf[0])
    {
        int offset = atoi(buf);
        if (offset < (EEPROM_SIZE/FLASH_SIZE) && offset >= 0)
        {
            WriteEEPROMAddr(offset);
            cmdMessenger.sendCmd(kACK,"EEPROM Address Updated.");
        }
        else
            cmdMessenger.sendCmd(kERR,"EEPROM Address is out of range");

    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}

void flash_write()
{
    WriteFlash();
    cmdMessenger.sendCmd(kACK,"EEPROM Written.");
    Blink();
}

void roach_power()
{
    char buf[2] = {'\0'};
    cmdMessenger.copyString(buf, 2);
    if(buf[0])
    {
        int state = atoi(buf);
        if (state == 0 || state == 1)
        {
            WriteRoach(state);
            cmdMessenger.sendCmd(kACK,"Roach Power State Written");
        }
        else
            cmdMessenger.sendCmd(kERR,"Roach Power State is not out of range");
    }
    else
        cmdMessenger.sendCmd(kERR,"Data string could not be parsed");
    Blink();
}        


void debug()
{
    if ( cmdMessenger.available() )
    {
        char buf[60] = { '\0' };
        cmdMessenger.copyString(buf, 60);
        if(buf[0])
            
            
            cmdMessenger.sendCmd(kACK,buf);
    }
    Blink();
}


// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void unknownCmd()
{
  // Default response for unknown commands and corrupt messages
    cmdMessenger.sendCmd(kERR,"Unknown command");
    Blink();
}

// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------




// ------------------ S E T U P ----------------------------------------------

void attach_callbacks(messengerCallbackFunction* callbacks)
{
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i])
  {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}

void setup() 
{
    // Listen on serial connection for messages from the pc
    Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
    /*Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB*/

    cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF,
                                     // and you wish to remove them
    /*cmdMessenger.print_LF_CR();   // Make output more readable whilst*/
                                     // debugging in Arduino Serial Monitor

    // Attach default / generic callback methods
    /*cmdMessenger.attach(kREADY, ready);*/
    cmdMessenger.attach(unknownCmd);

    // Attach defined callback methods
    attach_callbacks(messengerCallbacks);

    PinSetup();
    Blink();
}


// ------------------ M A I N ( ) --------------------------------------------

// Timeout handling
long timeoutInterval = 2000; // 2 seconds
long previousMillis = 0;
int counter = 0;

void timeout()
{
  counter++;
  digitalWrite(13,HIGH);
  delay(20);
  digitalWrite(13,LOW); 
}  

void loop() 
{
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();

  // handle timeout function, if any
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }

  // Loop.
}

