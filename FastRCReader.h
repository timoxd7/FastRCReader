/*
  FastRCReader for Arduino Nano and Uno to read fast and accurat RC Data.

  Written by Timo Meyer.
  The library is contributed as-is.

  To specify the right ports you want to use as input,
  scroll a bit down and select the right ones in the config part.
*/


//Directly accessing the Registers of the AVR-Processor. Only compatible with AVR-based boards
#ifndef __AVR__
#error "Sorry, the FastRCReader.h is optimised for Arduino AVR Boards (e.g. Uno, Nano...) Please use a different library"
#endif

//Just tested with ATmega328P and ATmega168, so just blocking the rest to prevent from missuse
#ifdef __AVR_ATmega328P__
#else
#ifdef __AVR_ATmega168__
#else
#error "Sorry, the library is currently only optimised for Atmega168 and Atmega328P boards."
#endif // nDefined __AVR_ATmega328P__
#endif // nDefined __AVR_ATmega168__

//--------------------------------------------------------------------------------------------------------------

#ifndef FastRCReader_h
#define FastRCReader_h

//----------------------------------Configuration----------------------------------

/* Choose one of the following to set the Ports wich will be used:
    1 for Pin D8 to D13
    2 for Pin A0 to A5 (Analog pins)
    3 for Pin D0 to D7
*/

#ifndef PORTS_TO_USE
#define PORTS_TO_USE 3 //Put in the needed number discribed above here
#endif

//----------------------------------End-Configuration-----------------------------------------------------------

#if PORTS_TO_USE == 1
//Interrupt vector definition
#define INTERRUPT_VECTOR PCINT0_vect
//How many ports are connectet to the board?
#define PORTCOUNT 6
//Port definitions (Bit-definitions) for the DDRx
const uint8_t portBit[PORTCOUNT] {PB0, PB1, PB2, PB3, PB4, PB5};
//Set the corresbonding bit for the Pin Change Interrupt Control Register (Register which enables Pin Change Interrupts)
#define PCICR_BIT PCIE0
//Set the corresbonding bit for the Pin Change Interrupt Flag Register (Register which tells if a interrupt got triggered)
#define PCIFR_BIT PCIF0
//Set the right register for the Pin Change Mask
#define PINCHANGEMASK PCMSK0
//Set the corresbonding bit for the Pin Change Mask
const uint8_t pcmskBit[PORTCOUNT] {PCINT0, PCINT1, PCINT2, PCINT3, PCINT4, PCINT5};
//Define the Port Input/Output Register
#define INPUT_OUTPUT_REGISTER DDRB
//Define the Port Output Data Register
#define OUTPUT_DATA_REGISTER PORTB
//Define the Pin Input Save Register
#define INPUT_DATA_REGISTER PINB
#else

//The same just for the other cases, so no commentary
#if PORTS_TO_USE == 2
#define INTERRUPT_VECTOR PCINT1_vect
#define PORTCOUNT 6
const uint8_t portBit[PORTCOUNT] {PC0, PC1, PC2, PC3, PC4, PC5};
#define PCICR_BIT PCIE1
#define PCIFR_BIT PCIF1
#define PINCHANGEMASK PCMSK1
const uint8_t pcmskBit[PORTCOUNT] {PCINT8, PCINT9, PCINT10, PCINT11, PCINT12, PCINT13};
//Define the Port Input/Output Register
#define INPUT_OUTPUT_REGISTER DDRC
//Define the Port Output Data Register
#define OUTPUT_DATA_REGISTER PORTC
//Define the Pin Input Save Register
#define INPUT_DATA_REGISTER PINC
#else

#if PORTS_TO_USE == 3
#define INTERRUPT_VECTOR PCINT2_vect
#define PORTCOUNT 8
const uint8_t portBit[PORTCOUNT] {PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7};
#define PCICR_BIT PCIE2
#define PCIFR_BIT PCIF2
#define PINCHANGEMASK PCMSK2
const uint8_t pcmskBit[PORTCOUNT] {PCINT16, PCINT17, PCINT18, PCINT19, PCINT20, PCINT21, PCINT22, PCINT23};
//Define the Port Input/Output Register
#define INPUT_OUTPUT_REGISTER DDRD
//Define the Port Output Data Register
#define OUTPUT_DATA_REGISTER PORTD
//Define the Pin Input Save Register
#define INPUT_DATA_REGISTER PIND
#else

#ifdef PORTS_TO_USE
#error "There is a wrong port definition in FastRCReader.h!"
#else
#error "There is no port definition in FastRCReader.h!"
#endif //def PORTS_TO_USE

#endif //3 / D
#endif //2 / C
#endif //1 / B

//--------------------------------------------------------------------------------------------------------------

//Global Vars/Interrupt sensitive vars
struct _channel { //needs to be global because of the Interrupt. Using struct, is more readable and faster as an object
  uint8_t active; //could also use bool array [8] but it uses for every bool one full byte, waste of ram. To save if the pin is active or not
  uint8_t lastStatus; //same as one line above. To save the last status from the last check
  uint16_t frequency[8]; //holds the Frequency or better say the Time, the channel is high. Direct proportional to the State of the stick of the RC
  uint16_t lastDown[8]; //last time the channel was LOW. To mesure the HIGH time
} _channel;


//----------------------------------Header----------------------------------

class FastRCReader
{
  public:

    void begin();

    void addChannel(uint8_t Channel);
    void addChannel(uint8_t Channels[]);
    void addChannel(uint8_t *Channels, uint8_t ChannelAmount);

    void stopChannel(uint8_t Channel);
    void stopChannel(uint8_t Channels[]);
    void stopChannel(uint8_t *Channels, uint8_t ChannelAmount);

    uint16_t getFreq(uint8_t Channel);
};

class RCChannelMapper : public FastRCReader
{
  public:

    void setMap(uint16_t minChannelState, uint16_t maxChannelState, uint8_t Channel, float toMin = -1, float toMax = 1);
    float getChannel(uint8_t Channel);

  protected:

    struct channel {
      uint16_t min[PORTCOUNT], max[PORTCOUNT];
      float toMin[PORTCOUNT], toMax[PORTCOUNT];
    } channel;
};

//----------------------------------CPP-FastRCReader----------------------------------

void FastRCReader::begin() {
  //Stop active interrups / delete global interrupt bit
  cli();

  //Delete the Flag in the Pin Change Interrupt Flag Register to prevent from false interrupt triggering
  PCIFR &= ~(1 << PCIFR_BIT);

  //Delete all corresbonding bits in the Pin Change Mask Register to prevent from fals interrupt triggering
  PINCHANGEMASK = 0b00000000;

  //Set bit in Pin Change Interrupt Control Register for the needed Ports
  PCICR |= (1 << PCICR_BIT);

  //Activate interrupt / set global interrupt bit
  sei();
}

void FastRCReader::addChannel(uint8_t ch) {
  //To map the Ports D8 to D13 at 0 to 5
#if PORTS_TO_USE == 1
  ch -= 8;
#endif // PORTS_TO_USE

  //Is the got Channel a valid Channel?
  if (ch < 0 || ch >= PORTCOUNT) return;

  _channel.active |= (1 << ch); //Activating bit accoding to its pin

  INPUT_OUTPUT_REGISTER &= ~(1 << portBit[ch]); //Set the needed pin as input
  PINCHANGEMASK |= (1 << pcmskBit[ch]); //Add interrupt for the needed Channel
}

void FastRCReader::addChannel(uint8_t ch[]) {
  addChannel(ch[0], sizeof(ch));
}

void FastRCReader::addChannel(uint8_t *ch, uint8_t channelAmount) {
  for (uint8_t i = 0; i < channelAmount; i++){
    addChannel(ch[i]);
  }
}

void FastRCReader::stopChannel(uint8_t ch) {
  //To map the Ports D8 to D13 at 0 to 5
#if PORTS_TO_USE == 1
  ch -= 8;
#endif // PORTS_TO_USE

  //Is the got Channel a valid Channel?
  if (ch < 0 || ch >= PORTCOUNT) return;

  _channel.active &= ~(1 << ch); //Deactivate channel

  PINCHANGEMASK &= ~(1 << pcmskBit[ch]); //Stop triggering interrupt by the given pin
  INPUT_OUTPUT_REGISTER |= (1 << portBit[ch]); //Set the pin which shoulden't be used anymore as output
}

void FastRCReader::stopChannel(uint8_t ch[]) {
  stopChannel(ch[0], sizeof(ch));
}

void FastRCReader::stopChannel(uint8_t *ch, uint8_t channelAmount) {
  for (uint8_t i = 0; i < channelAmount; i++){
    stopChannel(ch[i]);
  }
}

uint16_t FastRCReader::getFreq(uint8_t ch) {
  //To map the Ports D8 to D13 at 0 to 5
#if PORTS_TO_USE == 1
  ch -= 8;
#endif // PORTS_TO_USE

  //Is the got Channel a valid Channel?
  if (ch < 0 || ch >= PORTCOUNT) return 0;

  return _channel.frequency[ch];
}


ISR(INTERRUPT_VECTOR) { //Interrupt called once any of the pins gets changed
  //Save the current time / Time almost exactly after the event/portchange happened
  unsigned long currentTime = micros();

  //Do for every Port
  for (uint8_t i = 0; i < PORTCOUNT; i++) {
    //Channel active?
    if (_channel.active & (1 << i)) { //same as i would write _channel.active[i] by using a boolean array
      //Port HIGH?
      if (INPUT_DATA_REGISTER & (1 << portBit[i])) { //-> Port HIGH
        //Port LOW last time?
        if (~_channel.lastStatus & (1 << i)) {
          //Save new status
          _channel.lastStatus |= (1 << i);
          //Save time at last status change
          _channel.lastDown[i] = currentTime;
        }
        //Port HIGH last time -> do nothing
      } else { //-> Port LOW
        //Port HIGH last time?
        if (_channel.lastStatus & (1 << i)) {
          _channel.lastStatus &= ~(1 << i);
          _channel.frequency[i] = currentTime - _channel.lastDown[i];
        }
      }
    }
  }
}

//----------------------------------CPP-RCChannelMapper----------------------------------

void RCChannelMapper::setMap(uint16_t fromMin, uint16_t fromMax, uint8_t ch, float toMin = -1, float toMax = 1) {
  //To map the Ports D8 to D13 at 0 to 5
#if PORTS_TO_USE == 1
  ch -= 8;
#endif // PORTS_TO_USE

  //Is the got Channel a valid Channel?
  if (ch < 0 || ch >= PORTCOUNT) return;

  //Channel min and max equal?
  if (fromMin == fromMax) return;
  if (toMin == toMax) return;

  channel.min[ch] = fromMin;
  channel.max[ch] = fromMax;
  channel.toMin[ch] = toMin;
  channel.toMax[ch] = toMax;
}

float RCChannelMapper::getChannel(uint8_t ch) {
  //To map the Ports D8 to D13 at 0 to 5
#if PORTS_TO_USE == 1
  ch -= 8;
#endif // PORTS_TO_USE

  //Is the got Channel a valid Channel?
  if (ch < 0 || ch >= PORTCOUNT) return 0;

  return (_channel.frequency[ch] - (float)channel.min[ch]) * (channel.toMax[ch] - channel.toMin[ch]) / ((float)channel.max[ch] - (float)channel.min[ch]) + channel.toMin[ch];
}


#endif //FastRCReader_h
