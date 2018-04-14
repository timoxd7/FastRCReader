/*
   Simple sketch to read 5 channels from an RC an map them to % (float from 0 to 1)
   using Pin D3 to D7 for Channel 1 to 5
*/

#include <FastRCReader.h>

//Time definitions
#define REFRESHRATE 100
#define WAITAFTERCLICK 400

//Port definitions
#define BUTTONPIN 8
#define FROMCHANNELPIN 3
#define TOCHANNELPIN 7

RCChannelMapper RC;

//Fuction which should wait until the defined button got pressed. Also "debouncing" by only triggering a given times
//in a certain timeframe
void waitButtonClick(uint8_t pin) {
  //Saves last time the button got pressed. Static to be accessable also after the function got closed and reopened
  static unsigned long lastClickTime;

  //Wait to limit the times a butten can be pressed in a certain timeframe
  while ((millis() - lastClickTime) < WAITAFTERCLICK);

  //Wait until a button got pressed
  while (!digitalRead(pin));

  //Save the time the button got pressed
  lastClickTime = millis();
}


void setup() {
  pinMode(BUTTONPIN, INPUT_PULLUP);

  //Initalise the Serial communication and the RC RX Reader
  Serial.begin(115200);
  RC.begin();

  //Add/Activate all needed channels
  for (uint8_t i = FROMCHANNELPIN; i <= TOCHANNELPIN; i++) {
    RC.addChannel(i);
  }

  //Get the mins and the maxs of all channels to map them
  for (uint8_t i = FROMCHANNELPIN; i <= TOCHANNELPIN; i++) {
    waitButtonClick(BUTTONPIN);
    uint16_t channelMin = RC.getFreq(i);

    waitButtonClick(BUTTONPIN);
    uint16_t channelMax = RC.getFreq(i);

    RC.setMap(channelMin, channelMax, i);
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
