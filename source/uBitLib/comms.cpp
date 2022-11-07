#include "MicroBit.h"
#include "MicroBitSender.h"

void sendMessage(PacketBuffer data)
{
    uBit.radio.datagram.send(data);
}

PacketBuffer recvMessage()
{
    PacketBuffer data = uBit.radio.datagram.recv();
    return data;
}