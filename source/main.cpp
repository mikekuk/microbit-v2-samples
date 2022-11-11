#include "MicroBit.h"

MicroBit    uBit;

void onButtonA(MicroBitEvent e)
{
    // Serial pc (uBit.io.P11, uBit.io.P12);
    // pc.setBaudrate(9600);
    // pc.send('wind9999\n');

    uBit.display.scroll("1");
    uBit.serial.clearTxBuffer();
    uBit.serial.send("wind9999\n");
}

void onButtonB(MicroBitEvent e)
{
    
    uBit.display.scroll("2");
    uBit.serial.clearTxBuffer();
    uBit.serial.send("temp5555\n");
    
}

void onButtonAB(MicroBitEvent e)
{
    
    uBit.display.scroll("3");
    uBit.serial.clearTxBuffer();
    uBit.serial.send("solr1111\n");
}

int main()
{
    uBit.init();

    // Configure serial to work with Pi Pico
    uBit.serial.redirect(uBit.io.P11, uBit.io.P12);
    uBit.serial.setBaudrate(9600);

    
    //Set up our serial connection with a non-default baud rate & buffer sizes.
    
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onButtonAB);

    release_fiber();
    return 0;
}