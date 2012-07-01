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
  ready,            // 004 in this example
  read_config,      // 005 
  write,            // 006 
  NULL
};




// ------------------ C A L L B A C K  M E T H O D S -------------------------

void ready()
{
    cmdMessenger.sendCmd(kREADY,"READY");
    Blink();
}

void read_config()
{
    char payload[5] = {0xff,0x00,0xaf,0xdd,'\0'};
    BuildPayload(payload);
    cmdMessenger.sendCmd(kACK,payload,4);
    Blink();
}

void write()
{
    if ( cmdMessenger.available() )
    {
        char buf[6] = { '\0' };
        cmdMessenger.copyString(buf, 6);
        if(buf[0])
            if (ProcessPayload(buf))
                cmdMessenger.sendCmd(kACK,"Written");
            else
                cmdMessenger.sendCmd(kERR,"ChecksumFailure");
    }
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
    /*Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial*/
    Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB

    // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF,
                                     // and you wish to remove them
    // cmdMessenger.print_LF_CR();   // Make output more readable whilst
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

