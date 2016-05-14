/*
BMC_SBUS version 0.1.0 May 14th 2016 Stu Aitken
A simplified library for writing out serial servo data in Futaba SBUS format 

explictly written to control Blackmagic Design micro cameras, but shoud work generally

note for RC use various important flags such as failsafe have been omiited here
please refer to original source Libray in this case

based on FUTABA_SBUS library written by Michael Baker 
see: https://github.com/mikeshub/FUTABA_SBUS
also see : http://forum.arduino.cc/index.php/topic,99708.0.html

initial reverse engineering of sbus protocol by Uwe Gartmann 
see: https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/

SBUS ©FUTABA CORPORATION 2016. 
*/

#include "BMC_SBUS.h"

void BMC_SBUS::begin()
{
  //intialise private data arrays
  //sbus_data is formatted for correct serial output
  //note that the actual 11bit sbus data for each channel is embedded across multiple data bytes in a very stange order
  //byte 1 and bytes 24 and 25 should be left as is 
  //the first is a start byte, the last is a stop byte and the second last holds various flags
  //servos is the internal per channel position and is more straightforward - one int_16 per channel

  uint8_t loc_sbusData[25] = {0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
  int16_t loc_servos[18]   = {1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0,0};

  //setup serial port to transmit at 100k baud and use 1 parity and 2 stop bits

  SERIALPORT.begin(BAUDRATE, SERIAL_8E2);

  //setup public data arrays

  memcpy(sbusData,loc_sbusData,25);
  memcpy(servos,loc_servos,18);
}

void BMC_SBUS::Servo(uint8_t ch, int16_t position) 
{
  //set servo position on single channel

  if ((ch>0)&&(ch<=16)) 
  {
	  constrain (position, 0, 2048); //keep within min/max values
    servos[ch-1] = position; //expects a non zero starting index to the channel
  }
}

void BMC_SBUS::Send(void)
{
  //send data over serial port
  SERIALPORT.write(sbusData, 25); //according to docs for Serial we can send the array along as is without a loop
}

void BMC_SBUS::Update(void) 
{
  //update positions for all servo channels within the SBUS data frame
  //ignores digital servos and any failsafe mode stuff that was originally written

  //clear out existing sbus data for all channel data bytes
  //ignores first and last bytes in the array (start and stop bytes)
  //mapping loop relies on initial 0 values - do not omit this step!

  uint8_t i;
  for (i=1; i<24; i++) 
  {
    sbusData[i] = 0;
  }

  //reset counters

  ch = 0;
  bit_in_servo = 0;
  byte_in_sbus = 1;
  bit_in_sbus = 0;

  //format sbus data - maps sevo data array to sbus data array 1bit at a time
  //correctly deals with the little endian byte order in the process

  for (i=0; i<176; i++) //16channels*11bits = 176bits
  {
    if (servos[ch] & (1<<bit_in_servo)) //bitwise AND to check if the correct servo databit is set to 1
    {
      sbusData[byte_in_sbus] |= (1<<bit_in_sbus); //bitwise OR sets the correct sbus databit if true
    }

    //increment bit counters

    bit_in_sbus++;
    bit_in_servo++;

    //if end of sbus byte reset sbus bit counter and increment sbus byte counter

    if (bit_in_sbus == 8) 
    {
      bit_in_sbus = 0;
      byte_in_sbus++;
    }

    // if we have reached bit 11 in the servo data increment channel index and reset servo bit counter

    if (bit_in_servo == 11) 
    {
      bit_in_servo = 0;
      ch++;
    }
  }
}
