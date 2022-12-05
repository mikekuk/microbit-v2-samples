#include "cryptoTest.h"
#include "aes.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "MicroBit.h"
#include "MicroBitSender.h"
#include <time.h>
#include "sha256.h"
#include "crypto.h"
#include "hexlify.h"
#define ECB 1
#include "tiny-aes.hpp"
#include "tiny-aes.h"

/*
Salt generation function based on system time
*/
ManagedString generateSalt()
{
    //Get time in millesecond since last reset of MicroBit
    unsigned long rawtime = uBit.systemTime();
    int time = (int) rawtime;
    //Return the first 4 digits as the salt.
    return ((ManagedString)time + 1000).substring(0,4);
}


/*
Hash generation with SHA256
*/
ManagedString generateSHA256(ManagedString plain)
{
    //Casting of ManagedString to char array
    const char * input = plain.toCharArray();
    //Determining the SHA-256 hash. Function returns a C++ string
    std::string cppstring = sha256(input);
    //Casting C++ string to C-string
    const char * cstring = cppstring.c_str();
    //Casting of C-string to ManagedString
    ManagedString hash = (ManagedString)cstring;

    return hash;
}

/*
This function adds padding to a message so if needed to create a single plaintext block of 16 bytes
*/
ManagedString padMessage(ManagedString message)
{
    //ECB is a block cipher so we need to add padding to our string
    ManagedString pad = "****************";
    ManagedString paddedplain = message + pad;
    return paddedplain.substring(0,16);
}

/*
This function should take a string of the uBit type managed string and return the encrypted CharArray.
We set the maximum length of the message at 16 bytes for now.
*/
void encryptMessageECB(ManagedString plaintext, uint8_t key[32], uint8_t * buffer)
{
    // //debug fixed key test
    // uint8_t ecb_keys[4][32]={{0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
    //                 {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
    //                 {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
    //                 {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c}
    // };
    // uint8_t key2[32] = {0};
    // memcpy(key2, ecb_keys[2], 32);

    //Declaration of variables needed for encryption
    uint8_t plaindata[16] = {0};
    uint8_t encrypteddata[16] = {0};
    const char *p;
    
    //Converting our plaintext to the right format for our encryption function
    p = plaintext.toCharArray();
    
    uBit.serial.printf("\r\nplain data:");
    for(int i = 0; i < 16; i++)
    {
        plaindata[i] = (uint8_t)p[i];
        uBit.serial.printf("%x", plaindata[i]);
    }

    memcpy(encrypteddata, plaindata, 16);

    for(int i = 0; i < 16; i++)
    {
        plaindata[i] = (uint8_t)p[i];
    }

    aes_ecb(key, encrypteddata);

    for(int j = 0; j < 16; j++)
    {
        buffer[j] = encrypteddata[j];
    }

}

void decryptMessageECB(uint8_t key[32], uint8_t cryptdata[16], uint8_t * cryptbuffer)
{

    // //Declaration of variables needed for encryption
    uint8_t plaindata[16] = {0};
    uint8_t encrypteddata[16] = {0};
    uint8_t tinykey[32] ={};
 
    for(int i = 0; i < 32; i++)
    {
        tinykey[i] = key[i];
    }
    
    for(int i = 0; i < 16; i++)
    {
        encrypteddata[i] = cryptdata[i];
    }

    // uBit.serial.printf("\r\nkey used for decryption attempt:");
    // for(int i = 0; i < 32; i++)
    // {
    //     uBit.serial.printf("%x", key2[i]); 
    // }

    memcpy(plaindata, encrypteddata, 16);

    // uBit.serial.printf("\r\ndata trying to decrypt:");
    // for(int i = 0; i < 16; i++)
    // {
    //     uBit.serial.printf("%x", plaindata[i]); 
    // }
    
    memcpy(plaindata, encrypteddata, 16);

    // uBit.serial.printf("\r\nplain after dec:");
    // for(int i = 0; i < 16; i++)
    // {
    //     uBit.serial.printf("%x", plaindata[i]); 
    // }
    
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, tinykey);
    AES_ECB_decrypt(&ctx, plaindata);

    for(int j = 0; j < 16; j++)
    {
        cryptbuffer[j] = plaindata[j];
    }
}

PacketBuffer encrypt(ManagedString command, ManagedString salt)
{
    
    ManagedString padded_command = padMessage(command);
    ManagedString short_key_string = generateSHA256(getKey(salt));

    //Creating an array to hold our key and some casting to the right datatype.
    uint8_t finalhexedkey[32] = {0};
    
    //We need to unhexlify our sha key:
    const char *ptr_key = short_key_string.toCharArray();
    //Casting from const char to char
    char *shakey = (char*)ptr_key;
    //unhexlifying key with c++ library
    std::vector<unsigned char> unhexedkey = unhexlify(shakey);
    unsigned char* ptr_hexkey = &unhexedkey[0];

    for(int i = 0; i < 32; i++)
    {
        finalhexedkey[i] = (uint8_t)ptr_hexkey[i];
    }

    //Defining an array to hold the encrypted bytes.
    uint8_t enc[16] = {0};
    uint8_t *ptr_enc = enc;
    //Initiating encryption of our command.

    encryptMessageECB(padded_command, finalhexedkey, ptr_enc);

    PacketBuffer ctx(ptr_enc, 16);
    return ctx;
}

ManagedString decrypt(PacketBuffer crypttext)
{
    //Createing an array to hold our salt and some casting to the right datatype
    PacketBuffer temp(4);
    uint8_t cryptdata[16] = {0};
   
    for(int i = 0; i < 4; i++)
    {
        temp[i] = crypttext[i];
    }

    for(int i = 0; i < 16; i++)
    {
        cryptdata[i] = crypttext[i+4];
    }

    ManagedString salt = ManagedString( (const char *) temp.getBytes(), temp.length());

    uBit.serial.printf("\r\nafter enc: salt: ");
    uBit.serial.send(salt);

    ManagedString short_key_string = generateSHA256(getKey(salt));

    // uBit.serial.printf("\r\nkey before dec: ");
    // uBit.serial.send(hashedkey);

    //Creating an array to hold our key and some casting to the right datatype.
    uint8_t finalhexedkey[32] = {0};
    const char *ptr_key = short_key_string.toCharArray();

    char *shakey = (char*)ptr_key;

    std::vector<unsigned char> unhexedkey = unhexlify(shakey);
    unsigned char* ptr_hexkey = &unhexedkey[0];

    for(int i = 0; i < 32; i++)
    {
        finalhexedkey[i] = (uint8_t)ptr_hexkey[i]; 
    }

    //Defining an array to hold the decrypted bytes.
    uint8_t dec[16] = {0};
    uint8_t *ptr_dec = dec;

    //Initiating decryption of our command.
    decryptMessageECB(finalhexedkey, cryptdata, ptr_dec);
    PacketBuffer dtx(ptr_dec, 16);
    ManagedString plaintext = ManagedString( (const char *) dtx.getBytes(), dtx.length());
    return plaintext;
}

ManagedString get_secret()
{
    // Function that gets pre-shared secret value
    // TODO Write function
    ManagedString string = "SECRET";
    return string;
}

ManagedString getKey(ManagedString salt)
{
    return salt_secret(get_secret(), salt);
}

ManagedString salt_secret(ManagedString secret, ManagedString salt)
{
    // Function that salts secret and returns salted value
    return secret + salt;
}
