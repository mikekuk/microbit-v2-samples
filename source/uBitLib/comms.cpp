#include "MicroBit.h"
#include "MicroBitSender.h"
#include "crypto.h"
void onData(MicroBitEvent e)
{
    //ManagedString s = uBit.radio.datagram.recv();

    // if (s == "1")
    // {
    //     uBit.io.P0.setServoValue(0);
    //     uBit.display.print("A");
    // }

    // if (s == "2")
    // {
    //     uBit.io.P0.setServoValue(180);
    //     uBit.display.print("B");
    // }
}

void sendMessage(PacketBuffer data)
{
    uBit.radio.enable();
    uBit.radio.datagram.send(data);
}

PacketBuffer recvMessage()
{
    uBit.radio.enable();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    PacketBuffer data = uBit.radio.datagram.recv();
    return data;
}

void transmit(ManagedString message)   
{
    // Function that transmit message
    // TODO write function

    ManagedString salt = generateSalt();
    PacketBuffer ctx = encrypt(message, salt);
    
    PacketBuffer payload(20);

    for (int i = 0; i < 4; i++)
    {
        payload[i] = salt.charAt(i);
    }

    for (int i = 0; i < 16; i++)
    {
        payload[i+4] = ctx[i];
    }

    uint8_t *buf = payload.getBytes();

    uBit.serial.printf("\r\nTransmitting:\r\n");
    for (int i = 0; i < 20; i++)
    {
        uBit.serial.printf("%x ", *(buf + i));
    }
    uBit.serial.printf("\r\n\r\n");

    sendMessage(payload);
    


}
