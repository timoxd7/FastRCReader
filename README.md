# FastRCReader
A fast and lightweight library to read PWM RC signals from a RC RX (Receiver)


##Compatibility
Currently FastRCReader is only compatible with ATmega328P (Arduino UNO & NANO) and ATmega168 (Arduino NANO)

FastRCReader uses ISR (Interrupts). Do not use interrupts in your code or you will get an error somewhere.
Don't create a variable, object or struct with the name _channel, it is globally used by the FastRCReader.

##How to use
At the beginning, create a global Object for the FastRCReader:

    FastRCReader RC;

To initalize the RC, start the RC in the setup() function:

    RC.begin();

To add a Channel for your RC, use:

    addChannel(uint8_t Channel);

To delete a Channel, use:

    stopChannel(uint8_t Channel);

To define the ports you want to use, have a look in the FastRCReader.h
In the FastRCReader.h, use one of the following definitions (also described in there):

    Pick one of the following at "PORTS_TO_USE"
    Place 1 to use Port D8 - D13
    Place 2 to use Port A0 - A5
    Place 3 to use Port D0 - D7
