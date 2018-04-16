# FastRCReader
A fast and lightweight library to read PWM RC signals from a RC RX (Receiver)


## Compatibility
Currently FastRCReader is only compatible with ATmega328P (Arduino UNO & NANO) and ATmega168 (Arduino NANO)

FastRCReader uses ISR (Interrupts). Do not use interrupts in your code or you will get an error somewhere.
Don't create a variable, object or struct with the name _channel, it is globally used by the FastRCReader.

## How to use FastRCReader
At the beginning, create a global Object for the FastRCReader:

    FastRCReader RC;

To initalize the RC, start the RC in the setup() function:

    RC.begin();

To add a Channel for your RC, use:

    RC.addChannel(uint8_t Channel);

To delete a Channel, use:

    RC.stopChannel(uint8_t Channel);

To get the Frequency of a given Channel, use:

    RC.getFreq(uint8_t Channel);
    //Returns an unsigned int (uint16_t)

To define the ports you want to use, have a look in the FastRCReader.h. In the FastRCReader.h, use one of the following definitions (also described in there):

    Pick one of the following at "PORTS_TO_USE"
    Place 1 to use Port D8 - D13
    Place 2 to use Port A0 - A5
    Place 3 to use Port D0 - D7

## How to use RCChannelMapper
RCChannelMapper is a chield of FastRCReader, so you can use all commands described above. You can NOT use RCChannelMapper and FastRCReader at the same time! Only create one of both instances, RCChannelMapper does the same but more.

At the beginning, create a global Object for the RCChannelMapper:

    RCChannelMapper RC;

To initialize the RC, start the RC in the setup() function:

    RC.begin();

Add Channel & Delete Channel see above!

To set the Mapping, use:

    RC.setMap(uint16_t minChannelState, uint16_t maxChannelState, uint8_t Channel, float toMin = -1, float toMax = 1);

To get the mapped value of a Channel, use:

    RC.getChannel(uint8_t Channel);
    //Returns a float

## Pre-Calibration
If you want to pre-calibrate your RC before using with your own code, just use the Calibration_Mapping_to_EEPROM example. Just change the CHANNELAMOUNT definition and channelPins variable to your own needs. After calibrating your RC, the calibration will be saved in the EEPROM. Just add the following lines of code to your sketch to load it back again:

    #include <FastRCReader.h>
    #include <EEPROM.h>
    
    RCChannelMapper RC;
    #define EEPROMADRESS 0
    
    void setup(){
        RC.begin();
        EEPROM.get(EEPROMADRESS, RC);
    }

If you want to use the EEPROM for your own needs, you can also change the adress to match your own code (don't forget to also change the adress in the calibration example) with the EEPROMADRESS definition (by default 0, so just writing/reading to/from the beginning of the EEPROM).
