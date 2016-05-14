# BMC_SBUS

BMC_SBUS version 0.1.0 
May 14th 2016 
Stu Aitken

A simplified library for writing out serial servo data in Futaba SBUS format on the Arduino platform

explictly written to control Blackmagic Design micro cameras, but should work generally

note for RC use various important flags such as failsafe have been omitted here. Please refer to original source Libray in this case.

based on FUTABA_SBUS library written by Michael Baker - see: https://github.com/mikeshub/FUTABA_SBUS
also see: http://forum.arduino.cc/index.php/topic,99708.0.html

initial reverse engineering of sbus protocol by Uwe Gartmann 
see: https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/

SBUS ©FUTABA CORPORATION 

this library is based on the original work linked to above but simplified to provide straightforward methods to write channel data to the camera

to use, copy the BMC_SBUS folder to the the library folder for your Arduino installation and then restart the IDE
you should then be able to include BMC_SBUS in your sketches

to create a new instance:

`#include <BMC_SBUS.h>
BMC_SBUS mySBUS;`

then make sure you initialise this in the setup function of your sketch:

`void setup() {
  mySBUS.begin(); 
}`

the class has 3 methods:

`void mysbus.Servo(uint8_t ch, int16_t position)`
will set one of 16 channels to the given position value (valid position values are 0 to 2047)

`void mysbus.Update(void);`
will format all 16 servo channels into a valid sbus data frame

`void mysbus.Send(void);`
will send the currently defined frame out over the serial port

the serial port to use is defined in the header file and defaults to the standard Serial
you can change that to Serial1 or whatever if you have a board with more than 1 HW serial port

the library and example sketches are heavily commented so please take a look there for more info

