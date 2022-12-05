#include <vector>
#include <iostream>

/*
=================================================================================
*/
int ascii2val(char c)
{
    int iRetVal;

    if      ((c >= '0') && (c <= '9')) { iRetVal = (c - '0');      }
    else if ((c >= 'a') && (c <= 'f')) { iRetVal = (c - 'a' + 10); }
    else if ((c >= 'A') && (c <= 'F')) { iRetVal = (c - 'A' + 10); }
    else    { iRetVal = 0; }

    return iRetVal;
}

/*
=================================================================================
*/

std::vector<unsigned char> unhexlify(const std::string &InBuffer)
{
    std::vector<unsigned char> OutBuffer(InBuffer.size()/2 + 1);

    for(size_t i = 0, j = 0; i < InBuffer.size(); i += 2, ++j)
    {
        uint8_t *dest = &OutBuffer[j];
        *dest++ = (((ascii2val( InBuffer[i]) << 4) | (ascii2val( InBuffer[i + 1] ))));
    }

    return OutBuffer;
}


// #if 1

// /*
// ======================================================================================
// ================================== TESTS =============================================
// ======================================================================================
// */

// int main(int argc, char const *argv[])
// {
//     std::string s = "Hello World !!!";
//     std::cout << "Original:    " << s << std::endl;

//     // initialize the vector
//     ByteBuffer mybuffer(s.begin(), s.end());

//     // hexlify; now we have a string of hex representations of the original ByteBuffer
//     std::string stringResult = hexlify(mybuffer);
//     std::cout << "Hexlifyed:   " << stringResult  << std::endl;

// //----------------------------------

//     // unhexlify; we are back to a ByteBuffer
//     ByteBuffer bytebufferResult = unhexlify(stringResult);
//     // transfer from byte buffer to string in order to show it easier
//     std::string s1( bytebufferResult.begin(), bytebufferResult.end() );
//     std::cout << "Unhexlifyed: " << s1  << std::endl;

// // =================================

//     // hexlify; now we have a string of hex representations of the original ByteBuffer
//     std::string stringResult1 = hexlify(mybuffer, 0, 13);
//     std::cout << "Hexlifyed1:  " << stringResult1  << std::endl;

// //----------------------------------

//     // unhexlify; we are back to a ByteBuffer
//     ByteBuffer bytebufferResult1 = unhexlify(stringResult1);
//     // transfer from byte buffer to string in order to show it easier
//     std::string s2( bytebufferResult1.begin(), bytebufferResult1.end() );
//     std::cout << "Unhexlifyed2:" << s2  << std::endl;

//     return 0;
// }

// #endif

/*
$ bin/app 
Original:    Hello World !!!
Hexlifyed:   48656C6C6F20576F726C6420212121
Unhexlifyed: Hello World !!!
Hexlifyed1:  48656C6C6F20576F726C642021
Unhexlifyed2:Hello World !
*/