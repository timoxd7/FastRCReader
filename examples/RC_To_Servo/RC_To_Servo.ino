/*
   This sketch uses the calibration saved in the EEPROM (don't forget to calibrate your RC by using the
   calibration sketch) to control four Servos by using an RC
*/

#include <FastRCReader.h>
#include <EEPROM.h>
#include <Servo.h>

//Port definitions
#define CHANNELAMOUNT 4
const uint8_t channelPins[CHANNELAMOUNT] = {3, 4, 5, 7};

//Servos
const uint8_t servoPorts[CHANNELAMOUNT] = {6, 9, 10, 11};

//The adress the calibration/mapping is saved in the EEPROM
#define EEPROMADRESS 0

RCChannelMapper RC;
Servo servo[CHANNELAMOUNT];

void setup() {
  Serial.begin(115200);
  RC.begin();

  for (uint8_t i = 0; i < CHANNELAMOUNT; i++) {
    RC.addChannel(channelPins[i]);
    Serial.print("Added Channel "); Serial.print(i + 1); Serial.print(" on Port "); Serial.print(channelPins[i]);
    servo[i].attach(servoPorts[i]);
    Serial.print("\tActivated Servo "); Serial.print(i + 1); Serial.print(" on Port "); Serial.println(servoPorts[i]);
  }

  Serial.println("\nAdded/Attached all Ports\n");

  EEPROM.get(EEPROMADRESS, RC);

  Serial.println("Red from EEPROM, starting loop");

  /*
    unsigned long currentTime = millis();
    while((millis() - currentTime) < 3000);
  */
}

void loop() {
  for (uint8_t i = 0; i < CHANNELAMOUNT; i++) {
    float angle = (RC.getChannel(channelPins[i]) + 1) * 90;

    if (angle > 180) angle = 180;
    if (angle < 0)   angle = 0;

    servo[i].write(angle);
  }
}

