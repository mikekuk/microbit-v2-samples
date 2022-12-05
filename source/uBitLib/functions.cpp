#include "MicroBit.h"
#include "MicroBitSender.h"
#include "crypto.h"
#include "comms.h"
#include "cryptoTest.h"
#include "aes.h"

/* The AB control, has a rainbow function displayed on the breadboard using the Kitronik Inventor's kit, with a switch connected on Pin 8.. for an RGB LED  */
MicroBitPin P8(MICROBIT_ID_IO_P8, MICROBIT_PIN_P8, PIN_CAPABILITY_DIGITAL);

void onButtonA(MicroBitEvent)
{
    int temp_int = uBit.thermometer.getTemperature();
    ManagedString command = "temp";
    ManagedString value = (ManagedString) temp_int;
    ManagedString value_pad = "0000";
    ManagedString message = command + value_pad.substring(0, 4 - value.length()) + value;
    uBit.serial.printf("Temp: %d \r\n", temp_int);
    uBit.display.scroll("1");
    transmit(message);
}

void onButtonB(MicroBitEvent)
{
    MicroBitPin P2(MICROBIT_ID_IO_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_ANALOG);
    // Takes one second average reading with 0.1 second sample intervals.
    uint32_t samples = 0;
    uint16_t average = 0;
    uint16_t i = 0;
    while (i < 100)
    {
        samples += P2.getAnalogValue();
        uBit.sleep(10);
        i++;
    }
    average = (samples)/100;
    uBit.serial.printf("Wind Speed: %d \r\n", average);
    ManagedString command = "wind";
    ManagedString value = (ManagedString) average;
    ManagedString value_pad = "0000";
    ManagedString message = command + value_pad.substring(0, 4 - value.length()) + value;
    uBit.display.scroll("2");
    transmit(message);
}

void onButtonAB(MicroBitEvent)
{
    uint16_t lightLevel;
    
    // Sending images through serial - Sun & Moon
    MicroBitImage sun("255,0,255,0,255\n0,255,255,255,0\n255,255,255,255,255\n0,255,255,255,0\n255,0,255,0,255\n");
    MicroBitImage moon("255,255,255,0,0\n0,255,255,255,0\n0,0,255,255,0\n0,255,255,255,0\n255,255,255,0,0\n");
    // Change display mode to ambient light sensing
    uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE);                  
    lightLevel = uBit.display.readLightLevel(); 

    if (lightLevel < 200){

        uBit.display.scroll(moon);       
        uBit.display.scroll("OFF");    
        uBit.display.scroll(lightLevel); 
        uBit.sleep(300); //min time in ms between transmission, 500ms is  0.3sec!
    }  
    else{

        uBit.display.scroll(sun);   
        uBit.display.scroll("ON");  
        uBit.display.scroll(lightLevel);      
        uBit.sleep(300); //min time in ms between transmission, 500ms is  0.3sec!
        // rainbow(); Commented as currently erroring on build.
    }
   
    ManagedString command = "ligt";
    ManagedString value = (ManagedString) lightLevel;
    ManagedString value_pad = "0000";
    ManagedString message = command + value_pad.substring(0, 4 - value.length()) + value;
    uBit.display.scroll("3");
    transmit(message);
}

void debugMessageMicro()
{
    uBit.serial.send("\r\n\r\nDebug Functions: Input your option now ending with the return character\r\n");
    uBit.serial.send("\r\n    Select CARLO to perform the AES-256-ECB Monte Carlo Test (duration ~5 minutes)");
    uBit.serial.send("\r\n    Select TEST to test encryption by encrypting and decrypting 4 PTs with 4 keys in AES-256-ECB(duration ~10 seconds)");
    uBit.serial.send("\r\n    Select ECB to encrypt a string with AES-128-ECB and show the encoded hex values (duration ~10 seconds)");
    uBit.serial.send("\r\n    Select SALT to print a salt");
    uBit.serial.send("\r\n    Select HASH to print a hash (SHA-256) of the string IoT is great!");
    uBit.serial.send("\r\n    Select SEND to send an encrypted command over the radio interface\r\n");
    uBit.serial.send("\r\nMain Functions:\r\n");
    uBit.serial.send("\r\n    Select MAIN");
    uBit.serial.send("\r\n        Then push the left button to execute wind measurement");
    uBit.serial.send("\r\n        Then push the right button to execute temperature measurement");
    uBit.serial.send("\r\n        Then push both buttons to execute light measurement\r\n");
}

void debugFunctionMicro(ManagedString str)
{
    //Performs the Monte Carlo test for AES-256-ECB
    if(str == "CARLO")
    {
        uBit.serial.printf("\r\n Starting Monte Carlo loop");
        //Initiate test
        int equ = monteCarlo();
        //Output result to serial
        uBit.serial.printf("\r\nAES-%d ECB mode for %d-bit CPU : %s\n",
        AES_KEY_LEN*8, AES_INT_LEN*8, equ ? "OK" : "FAILED");
    }
    //Test the encryption implementation by encrypting plaintexts and comparing them to the expected outcome.
    else if(str == "TEST")
    {
        uBit.serial.printf("\r\n**** AES-256 ECB Test ****\r\n");
        //Initiate test
        int equ2 = testEncryption();
        //Output result to serial
        uBit.serial.printf("\r\n\r\nAES-256 ECB Test: %s\r\n", equ2 ? "OK" : "FAILED");
    }
    //Encrypt a string with a fixed key in AES-128-ECB
    else if(str == "ECB")
    {
        //Define a hardcode key and prepare plaintext string variable
        uint8_t fixedkey[32] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
        ManagedString plaintext;
            
        //Show the user the key:
        uBit.serial.send("\n\rThe fixed key is:\n\r");
        for(int i = 0; i < 32; i++)
        {   
            uBit.serial.printf("%x ", fixedkey[i]);
        }

        //ask user to enter a short plaintext
        uBit.serial.send("\r\n\r\nEnter a string (max length 16) ending with a return character:\r\n");
        plaintext=uBit.serial.readUntil("\r\n");

        uint8_t enc[16] ={0};
        uint8_t * ptr = enc;
        encryptMessageECB(plaintext, fixedkey, ptr);
    }
    //Generate a salt base on the time in milliseconds since last reset.
    else if(str == "SALT")
    {
        ManagedString salt;
        salt = generateSalt();
        uBit.serial.send("\r\n\r\nThe generated salt =\r\n");
        uBit.serial.send(salt);
    }
    //Generate a SHA-256 digest based on a fixed string.
    else if(str == "HASH")
    {
        ManagedString plainstring = "IoT is great!";
        ManagedString results;
        results = generateSHA256(plainstring);
        uBit.serial.send("\r\n\r\nThe generated hash of the string 'IoT is great!' =\r\n");
        uBit.serial.send(results);   
    }
    //Takes a string from the user, appands the salt and determines the SHA-256 digest. It uses this as a key to encrypt a message. The salt is appended to the encrypted message and the message is sent.
    else if (str == "SEND")
    {
        //Asking user to enter command and a secret string.
        uBit.serial.send("\r\n\r\nEnter the command to send (max length 16) ending with a return character:\r\n");
        ManagedString command = uBit.serial.readUntil("\r\n");
        uBit.serial.send("U entered:\r\n");
        uBit.serial.send(command);

        uBit.serial.send("\r\n\r\nEnter a secret string (max length 16) ending with a return character:\r\n");
        ManagedString secret_string = uBit.serial.readUntil("\r\n");
        uBit.serial.send("U entered:\r\n");
        uBit.serial.send(secret_string);

        uBit.serial.send("\r\n\r\nAppending salt to secret...\r\n");

        //Generate salt and append it to the secret string
        ManagedString salt = generateSalt();
        ManagedString salted_secret = secret_string + salt;
        //Display final secret string to user
        uBit.serial.send("\r\nBefore hashing:\r\n");
        uBit.serial.send(salted_secret);
        uBit.serial.send("\r\nAfter hashing:\r\n");
        //Hasing the secret.
        ManagedString hashed_secret = generateSHA256(salted_secret);
        uBit.serial.send(hashed_secret);

        uBit.serial.send("\r\n\r\nEncrypting command with 32 bytes of hashed secret+salt as the key\r\n");

        //Creating an array to hold our key and some casting to the right datatype.
        uint8_t key[32] = {0};
        const char *p = hashed_secret.toCharArray();

        for(int i = 0; i < 32; i++)
        {
            key[i] = (uint8_t)p[i];
        }

        //Defining an array to hold the encrypted bytes. 
        uint8_t enc[16] = {0};
        uint8_t *ptr = enc;
        //Initiating encryption of our command.
        encryptMessageECB(command, key, ptr);

        //Showing the encrypted bytes to the user
        uBit.serial.printf("\n\rThe encrypted bytes (in hex) are:\n\r");

        for(int j = 0; j < 16; j++)
        {
            uBit.serial.printf("%x ", enc[j]);
        }
        //Broadcasting our encrypted message.
        PacketBuffer b(enc,16);
        sendMessage(b);
    }
    return;
}

/***********************************************************
*
* Function: rainbow - true rainbow converts HSL mode angles 
                      to RGB
*
* Description: Activates Rainbow function
*
**********************************************************/

void setRGBpoint(uint8_t red, uint8_t green, uint8_t blue)
{
    // this code is for common anode LEDs
    // Pins 0, 1 , 2 on the breadboard are connected to an RGB led
     uBit.io.P0.setAnalogValue(red);
     uBit.io.P1.setAnalogValue(green);
     uBit.io.P2.setAnalogValue(blue);
}

// void rainbow()
// {
//     // Perform RGB actions
//     create_fiber(rainbow);   

//     //initialise Rainbow
//     uBit.display.scrollAsync("Rainbow");
   
//     // initialize the degree to 360
//     int degree = degree/360*255;
    
//     // initialize the colours to 0 to start with
//     uint8_t red, green, blue = 0;
   
//     // degrees of colour values
//     const uint8_t lights[61] = {0, 4, 8, 13, 17, 21, 25, 30, 34, 38, 42, 
//     47, 51, 55, 59, 64, 68, 72, 76, 81, 85, 89, 93, 98, 102, 106, 110, 115, 
//     119, 123, 127, 132, 136, 140, 144, 149, 153, 157, 161, 166, 170, 174, 178, 
//     183, 187, 191, 195, 200, 204, 208,212, 217, 221, 225, 229, 234, 238, 242, 
//     246, 251, 255};
   
//     // loop to swap colours
//     while (P8.getDigitalValue() == 1){

//         for(int i=0; i < 360; i++){

//             degree = i;

//             if (degree<60)  {red = 255; green = lights[degree]; blue = 0;} else
//             if (degree<120) {red = lights[120-degree]; green = 255; blue = 0;} else 
//             if (degree<180) {red = 0, green = 255; blue = lights[degree-120];} else 
//             if (degree<240) {red = 0, green = lights[240-degree]; blue = 255;} else 
//             if (degree<300) {red = lights[degree-240], green = 0; blue = 255;} else 
//                             {red = 255, green = 0; blue = lights[360-degree];}                
               
//             setRGBpoint(red, green, blue); // setRGBpoint to the pins
//             uBit.sleep(50);
//             }
//       }

//      //clear uBit threads, typically never reached
//     release_fiber();      
// }
