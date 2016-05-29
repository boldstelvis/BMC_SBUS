#include <BMC_SBUS.h>
BMC_SBUS mySBUS; 

/*
sketch uses a single mode button which flicks between 4 different modes
LED will blink to show mode when changed
control of one 'switch channel and one 'analog' channel available in each mode
UP and DOWN buttons change 'switched' values (channels 5 to 8)
10k POT sets 'analog' values directly (channels 1 to 4)

Channel setup per mode - map BMMCC camera functions to sbus channels as below

MODE 1
ch01: Focus   (controlled by pot)
ch05: REC     (controlled by up/down buttons)
MODE 2
ch02: Iris    (controlled by pot)
ch06: Shutter (controlled by up/down buttons)
MODE 3
ch03: Zoom    (controlled by pot)
ch07: ISO     (controlled by up/down buttons)
MODE 4
ch04: Levels  (controlled by pot)
ch08: WB      (controlled by up/down buttons)
*/

//set initial mode 
byte mode                 = 1;

//set IO pins
const byte pinMODE        = 10; 
const byte pinUP          = 11;
const byte pinDOWN        = 12;
const byte pinPOT         = 1; //analog pin!
const byte pinLED         = 13;

//set button press & debounce checks
byte statusMODE;
byte statusUP;
byte statusDOWN;
byte chkMODE              = 0;
byte chkUP                = 0;
byte chkDOWN              = 0;

//constants used to define low, neutral and high values for switch mode, MIN/MAX range for analog mode & frame delay
//try playing with these values to see what works

const int sbusMID         = 1024;   //neutral val
const int sbusLOW         = 0;      //low switch val
const int sbusHIGH        = 2000;   //high switch val
const int sbusMIN         = 300;    //min usable analog val
const int sbusMAX         = 1748;   //max usable analog val
const int sbusWAIT        = 4;      //frame timing delay in msecs


void setup() 
{
  //setup digital pin states
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW);
  pinMode(pinMODE, INPUT_PULLUP);
  pinMode(pinUP, INPUT_PULLUP);
  pinMode(pinDOWN, INPUT_PULLUP);
  //initialise sbus
  mySBUS.begin(); 
}

void loop() 
{
  //read buttons - LOW = pressed
  statusMODE = digitalRead(pinMODE);
  statusUP   = digitalRead(pinUP);
  statusDOWN = digitalRead(pinDOWN);

  //process button input with some checks for previous state to debounce

  //****MODE CHANGE****
  
  if (statusMODE == LOW && chkMODE == 0)
  {  
    mode++; 
    if (mode > 4) mode = 1; //cycle mode
    
    //blink LED to feedback mode
    for(int i=1; i <= mode; i++)
    {
      digitalWrite(pinLED, HIGH);
      delay(200);
      digitalWrite(pinLED, LOW);
      delay(200);
    }
    
    chkMODE = 1; //for debounce
  }
  
  else if (statusMODE == HIGH && chkMODE == 1) 
  {
    chkMODE = 0; //for debounce
  }

  // ****SWITCH CHANNEL INPUT****
  // switch mode on channels 5 to 8
  
  if (statusUP == LOW && chkUP == 0) 
  {
    //step UP camera function setting
    //sends out 2 frames immediately to create switch effect
    
    mySBUS.Servo((mode+4), sbusMID);
    mySBUS.Update();
    mySBUS.Send();
    
    delay(sbusWAIT); //frame timing
    
    mySBUS.Servo((mode+4), sbusHIGH);
    mySBUS.Update();
    mySBUS.Send();
    
    chkUP = 1; //for debounce
  }
  
  else if (statusUP == HIGH && chkUP == 1) 
  {
    chkUP = 0; //for debounce
  }
  
  if (statusDOWN == LOW && chkDOWN == 0) 
  {
    //step DOWN camera function setting
    //sends out 2 frames immediately to create switch effect
    
    mySBUS.Servo((mode+4), sbusMID);
    mySBUS.Update();
    mySBUS.Send();
    
    delay(sbusWAIT); //frame timing
    
    mySBUS.Servo((mode+4), sbusLOW);
    mySBUS.Update();
    mySBUS.Send();
    
    chkDOWN = 1; //for debounce
  }
  
  else if (statusDOWN == HIGH && chkDOWN == 1) 
  {
    chkDOWN = 0; //for debounce
  }

  // ****ANALOG CHANNEL INPUT****
  // analog mode on channels 1 to 4
  
  // reads the pot and maps the val to servo MIN/MAX range
  int potValue = analogRead(pinPOT);
  int servoValue;
  servoValue = map(potValue, 0, 1024, sbusMIN, sbusMAX+1);
  constrain (servoValue, sbusMIN, sbusMAX);

  //update sbus frame data
  mySBUS.Servo(mode, servoValue); //update servo position for active analog mode
  mySBUS.Update();
  mySBUS.Send(); 
  delay(sbusWAIT); //frame timing
}

