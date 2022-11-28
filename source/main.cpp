#include "MicroBit.h"

MicroBit    uBit;

void onData(MicroBitEvent e)
{
    PacketBuffer p = uBit.radio.datagram.recv();

    for (int i = 0; i < 16; i++)
    {
        // uBit.display.scroll(p.getByte(i));
        uBit.serial.printf("%x - ", p.getByte(i));
    }

    release_fiber();
    
}

int main()
{
    uBit.init();
    uBit.serial.setBaudrate(9600);

    uBit.serial.printf("XXXX Working XXXX\r\n\r\n");

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    while(1)
        uBit.sleep(1000);
}