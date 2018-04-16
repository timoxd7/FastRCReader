/*
   This sketch can be used to calibrate your RC to be mapped from -1 to 1. The calibration is then saved
   in the EEPROM to be used in a different sketch withyout the need of burning the calibration in your code.

   To use different ports as the one i used, just type your ports in under "channelPins". The total amount
   of pins you are using is to be defined under "CHANNELAMOUNT".
*/

#include "FastRCReader.h"
#include <EEPROM.h>

//Port definitions
//Change with the Ports you want to use
#define CHANNELAMOUNT 4
const uint8_t channelPins[PORTCOUNT] = {3, 4, 5, 7};

//The adress the calibration/mapping will be saved in the EEPROM
#define EEPROMADRESS 0

RCChannelMapper RC;

//Fuction which should wait until the next Calibration point is triggeret by serial
void waitSerial() {
  while (!Serial.available());

  while (true) {
    while (Serial.available()) {
      char got = Serial.read();
      if (got == 'N' || got == 'n') return;
    }
  }
}

void setup() {
  Serial.begin(115200);
  RC.begin();

  for (uint8_t i = 0; i < CHANNELAMOUNT; i++) {
    RC.addChannel(channelPins[i]);
  }

  //Get the mins and the maxs of all channels to map them
  for (uint8_t i = 0; i < CHANNELAMOUNT; i++) {
    //Write what is to do in Serial Monitor
    char buffer[57];
    sprintf(buffer, "Put Channel %i on Pin %i to Min position, then write N ", i + 1, channelPins[i]);
    Serial.print(buffer);

    //Then save Min value
    waitSerial();
    uint16_t channelMin = RC.getFreq(channelPins[i]);
    Serial.println(channelMin);

    //Same again
    sprintf(buffer, "Put Channel %i on Pin %i to Max position, then write N ", i + 1, channelPins[i]);
    Serial.print(buffer);

    waitSerial();
    uint16_t channelMax = RC.getFreq(channelPins[i]);
    Serial.println(channelMax);

    RC.setMap(channelMin, channelMax, channelPins[i]);
  }

  //Save to EEPROM
  EEPROM.put(EEPROMADRESS, RC);
  Serial.print("\nSaved in EEPROM, you can now run your own code by reading the RC-Object from the EEPROM");

  unsigned long currentTime = millis();
  while ((millis() - currentTime) < 3000);
}

void loop() {
  for (uint8_t i = 0; i < CHANNELAMOUNT; i++) {
    float gotX = RC.getChannel(channelPins[i]);
    Serial.print(gotX); Serial.print("\t");
  }

  Serial.println("");

  unsigned long currentTime = millis();
  while ((millis() - currentTime) < 20);
}

