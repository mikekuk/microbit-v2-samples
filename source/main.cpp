#include "aes.h"
#include "MicroBit.h"
#include "cryptoTest.h"
#include <string.h>
#include "MicroBitSender.h"
#include "crypto.h"
#include "comms.h"
#include "functions.h"
MicroBit uBit;

void onRx(MicroBitEvent e)
{
    PacketBuffer p = uBit.radio.datagram.recv();

    for (int i = 0; i < 20; i++)
    {
        // uBit.display.scroll(p.getByte(i));
        // uBit.serial.printf("%x - ", p.getByte(i));
        uBit.serial.send(p.getByte(i));
        uBit.serial.send(" - ");
    }
    uBit.serial.printf("\r\n");

    // Extract Salt form received message
    ManagedString salt;
    for (int i = 0; i < 4; i++)
    {
        salt = salt + (ManagedString) p.getByte(i);
    }
   
    // Extract command from received message
    ManagedString command;
      for (int i = 4; i < 20; i++)
    {
        command = command + (ManagedString) p.getByte(i);
    }
    
   
    // Get key from salt
    ManagedString key = getKey(salt);


    uBit.serial.send("Salt: ");
    uBit.serial.send(salt);
    uBit.serial.printf("\r\n");

    uBit.serial.send("key: ");
    uBit.serial.send(key);
    uBit.serial.printf("\r\n");


    ManagedString clearText;
    clearText = decrypt(p);

    uBit.serial.send("Clear Text: ");
    uBit.serial.send(clearText.substring(0,8));
    uBit.serial.printf("\r\n");

}

int main()
{
    uBit.init();
    
    uBit.serial.setBaudrate(9600);

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onRx);
    uBit.radio.enable();

    while(1)
        uBit.sleep(1000);

    release_fiber();
}