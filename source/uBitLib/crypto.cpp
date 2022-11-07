#include "cryptoTest.h"
#include "aes.h"
#include <string.h>
#include "MicroBit.h"
#include "MicroBitSender.h"
#include <time.h>
#include "sha256.h"

/*
Salt generation function based on system time
*/
ManagedString generateSalt()
{
    unsigned long rawtime = uBit.systemTime();
    int time = (int) rawtime;
    return (ManagedString) time;
}

/*
Hash generation with SHA256
*/
ManagedString generateSHA256(ManagedString plain)
{
    const char * input = plain.toCharArray();
    std::string cppstring = sha256(input);
    const char * cstring = cppstring.c_str();
    ManagedString hash = (ManagedString)cstring;

    return hash;
}


/*
This function should take a string of the uBit type managed string and return the encrypted CharArray.
We set the maximum length of the message at 16 bytes for now.
*/
void encryptMessageECB(ManagedString plaintext, uint8_t key[32], uint8_t * buffer)
{
    //concept 1: convert the string to an array and print the array to serial
    uint8_t plaindata[16] = {0};
    uint8_t encrypteddata[16] = {0};
    const char *p;

    //ECB is a block cipher so we need to add padding to our string
    ManagedString pad = "****************";
    ManagedString paddedplain = plaintext + pad;
    ManagedString finalplain = paddedplain.substring(0,16);

    uBit.serial.printf("\n\rYou entered the following string (includes padding): ");
    uBit.serial.send(finalplain);

    p = finalplain.toCharArray();

    uBit.serial.printf("\n\rAfter conversion to uint8_t: \r\n");
    for(int i = 0; i < 16; i++)
    {
        plaindata[i] = (uint8_t)p[i];
        uBit.serial.printf("%x ", plaindata[i]);
    }

    memcpy(encrypteddata, plaindata, 16);

    aes_ecb(key, encrypteddata);

    uBit.serial.printf("\n\rThe encrypted bytes (in hex) are:\n\r");

    for(int j = 0; j < 16; j++)
    {
        buffer[j] = encrypteddata[j];
        uBit.serial.printf("%x ", encrypteddata[j]);
    }
}