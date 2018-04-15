/*
   Simple sketch to read 5 channels from an RC an map them to % (float from 0 to 1)
   using Pin D3 to D7 for Channel 1 to 5

   At first, you have to calibrate/map your RC to -1 to 1
   To do so, connect the needed Channels to the Arduino (pins discribed above) and than upload
   the Sketch.

   After that, open the Serial Monitor. Then calibrate your RC like it is asked from the
   Serial Monitor.

   If you have done the calibration, type Y to save the calibration into
   the EEPROM (non volitale storage).

   Then close the Serial Monitor and open the Serial Plotter (under Tools->Serial Plotter)

   Tada!


   If you want to change the Pins you want to use, you can simply change the pin definitions
   down below (don't forget to recalibrate your RC then with the new channels if you have
   calibrated it before! Just write "true" at "RECALIBRATE". Then reupload->override EEPROM (by
   recalibrating)->set "RECALIBRATE" back to false->reupload. Finish!).
   
   But don't forget to use the right pin definition in the FastRCReader.h (At PINS_TO_USE)!
*/

#include "FastRCReader.h"
#include <EEPROM.h>

#define RECALIBRATE false

//Time definitions
#define REFRESHRATE 2
#define WAITAFTERCLICK 400

//Port definitions
#define FROMCHANNELPIN 3
#define TOCHANNELPIN 7

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


//Function wich translates a Y/y and N/n into true and false to ask Yes and No questions
//over Serial Monitor
bool waitSerialYesNo() {
  while (!Serial.available());

  while (true) {
    while (Serial.available()) {
      char got = Serial.read();
      if (got == 'Y' || got == 'y') return true;
      if (got == 'N' || got == 'n') return false;
    }
  }
}


void setup() {
  //Initalise the Serial communication and the RC RX Reader
  Serial.begin(115200);
  RC.begin();

  //Add/Activate all needed channels
  for (uint8_t i = FROMCHANNELPIN; i <= TOCHANNELPIN; i++) {
    RC.addChannel(i);
  }

  //Using Calibration from EEPROM?
  bool useFromEEPROM = false;

  if ((byte)EEPROM.read(0) == 42) { //Why? 42! (1 could be written from another program, 42 mostly not)
    //To use it with the Serial Plotter after calibrating it with the Serial Monitor, don't ask
    //(down below)
    if (RECALIBRATE){
      Serial.println("Using calibration from EEPROM (Y/N) ?");
      useFromEEPROM = waitSerialYesNo();
    } else {
      useFromEEPROM = true;
    }
  }

  //Use calibration saved in the EEPROM?
  if (useFromEEPROM) {
    EEPROM.get(1, RC);
  } else {
    //Get the mins and the maxs of all channels to map them
    for (uint8_t i = FROMCHANNELPIN; i <= TOCHANNELPIN; i++) {
      //Write what is to do in Serial Monitor
      char buffer[55];
      sprintf(buffer, "Put Channel %i to Min position, then write N ", i);
      Serial.print(buffer);

      //Then save Min value
      waitSerial();
      uint16_t channelMin = RC.getFreq(i);
      Serial.println(channelMin);

      //Same again
      sprintf(buffer, "Put Channel %i to Max position, then write N ", i);
      Serial.print(buffer);

      waitSerial();
      uint16_t channelMax = RC.getFreq(i);
      Serial.println(channelMax);

      RC.setMap(channelMin, channelMax, i);
    }

    Serial.println("Save calibration to EEPROM (Y/N) ?");

    //If yes, save calibration to EEPROM
    if (waitSerialYesNo()) {
      EEPROM.put(0, (byte)42);
      EEPROM.put(1, RC);
      Serial.print("Saved!");

      //Wait a second
      unsigned long waitSince = millis();
      while ((millis() - waitSince) < 1000);
    }
  }


}


void loop() {
  //Plot all Channels to the serial Plotter
  for (uint8_t i = FROMCHANNELPIN; i < TOCHANNELPIN; i++) {
    Serial.print(RC.getChannel(i)); Serial.print("\t");
  } Serial.println(RC.getChannel(TOCHANNELPIN));

  //Wait till the refreshrate is expiered
  unsigned long waitSince = millis();
  while ((millis() - waitSince) < REFRESHRATE);
}
