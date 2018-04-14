/*
   Simple sketch to read 5 RC Channels connectet to Pin D3 - D7

   If you want to change the Pins you want to use, you can simply change the pin definitions
   down below. But don't forget to use the right pin definition in the FastRCReader.h (At PINS_TO_USE)!
*/

#include <FastRCReader.h>

//Time definitions
#define REFRESHRATE 100

//Port definitions
#define FROMCHANNELPIN 3
#define TOCHANNELPIN 7

FastRCReader RC;

void setup() {
  //Initalise the Serial communication and the RC RX Reader
  Serial.begin(115200);
  RC.begin();

  //Add/Activate all needed channels
  for (uint8_t i = FROMCHANNELPIN; i <= TOCHANNELPIN; i++) {
    RC.addChannel(i);
  }
}

void loop() {
  //Plot all Channels to the serial Plotter
  for (uint8_t i = FROMCHANNELPIN; i < TOCHANNELPIN; i++) {
    Serial.print(RC.getFreq(i)); Serial.print("\t");
  } Serial.println(RC.getFreq(TOCHANNELPIN));

  //Wait till the refreshrate is expiered
  unsigned long waitSince = millis();
  while ((millis() - waitSince) < REFRESHRATE);
}
