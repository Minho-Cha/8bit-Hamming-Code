//
#include <stdio.h>
// 32bit
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int

// 11   10   9    8    7    6    5    4    3    2    1    0
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
//------------------------------------------------------------
//       BLOCK2                BLOCK1         BLOCK0
//--------------------    ----------------    ------         
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |
//--------------------    ----------------    ------
// P0
//    ------    ------    ------    ------    ------    ======    
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 0101 0101 0100 = 0x0554
//    ------    ------    ------    ------    ------    ======
//                        -----     -----------    -----------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b0101 1011 = 0x5B
//                        -----     -----------    -----------
// P1
//    -----------         -----------         -----======         
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 0110 0110 0100 = 0x0664
//    -----------         -----------         -----======         
//                        -----------    -----------    ------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b0110 1101 = 0x6D
//                        -----------    -----------    ------ 
// P2
//-----                   ---------------======                    
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 1000 0111 0000 = 0x0870
//-----                   ---------------======                    
//                   ------              ----------------     
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b1000 1110 = 0x8E
//                   ------              ----------------     
// P3
//-------------------======                                   
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 1111 0000 0000 = 0x0F00
//-------------------======                                       
//                   ---------------------                        
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b1111 0000 = 0xF0
//                   ---------------------                        
#define P0_BIT  0
#define P1_BIT  1
#define P2_BIT  3
#define P3_BIT  7

#define BLOCK0 0x01
#define BLOCK1 0x0E
#define BLOCK2 0xF0

#define BLOCK0_SHIFT 2
#define BLOCK1_SHIFT 3
#define BLOCK2_SHIFT 4

#define P0_POSI_12 0x0554
#define P1_POSI_12 0x0664
#define P2_POSI_12 0x0870
#define P3_POSI_12 0x0F00

#define P0_POSI_8 0x5B
#define P1_POSI_8 0x6D
#define P2_POSI_8 0x8E
#define P3_POSI_8 0xF0


#define PACKET_SIZE 10
#define NONE_ERROR 0

// Only a single bit error is possible
#define NOISE_BIT 13
#define NOISE_DATA 0x0003
#if 0
    #define NOISE (1 << NOISE_BIT)
#else
    #define  NOISE NOISE_DATA
#endif
uint16_t enHamming(uint8_t data);
uint8_t getEvenParityforTx(uint8_t data);
uint8_t findHammingError(uint16_t hamming);
uint8_t getEvenParityforRx(uint16_t data);
uint16_t fixError(uint16_t hammingData, uint8_t errorCode);
uint8_t deHamming(uint16_t hammingData);

//Hamming Test Code
int main(void)
{
    uint8_t arrTxData[PACKET_SIZE] = { 1, 2, 3, 4 ,5 ,6, 7, 8, 9, 10 };
    uint16_t hammingData[PACKET_SIZE] = { 0, };
    uint16_t errRxData[PACKET_SIZE] = { 0, };
    uint8_t errCode[PACKET_SIZE] = { 0, };
    uint16_t fixData[PACKET_SIZE] = { 0, };
    uint8_t deData[PACKET_SIZE] = { 0, };


    uint16_t temp = 0;

    //print original data
    printf("Original Data :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        printf("%02X, ", arrTxData[i]);
    }
    printf("\n\r");

    //encoding hamming
    printf("encoding Data :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        hammingData[i] = enHamming(arrTxData[i]);
        printf("%0X, ", hammingData[i]);
    }
    printf("\n\r");

    // Set Noise
    printf("Noise Data    :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        errRxData[i] = hammingData[i] | NOISE;
        printf("%0X, ", errRxData[i]);
    }
    printf("\n\r");


    // find error
    printf("Error code    :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        errCode[i] = findHammingError(errRxData[i]);
        printf("%02X, ", errCode[i]);
    }
    printf("\n\r");

    // fix error
    printf("fixed Data    :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        fixData[i] = fixError(errRxData[i], errCode[i]);
        printf("%0X, ", fixData[i]);
    }
    printf("\n\r");

    // decoding hamming
    printf("decoding Data :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        deData[i] = deHamming(fixData[i]);
        printf("%02X, ", deData[i]);
    }
    printf("\n\r");

    //Compare Original vs decoding data
    int cnt = 0;

    printf("------------------------------------------------------\n\rResult :");
    for (int i = 0; i < PACKET_SIZE; i++)
    {
        if (arrTxData[i] == deData[i])
        {
            cnt++;
        }
    }
    if (cnt == PACKET_SIZE)
    {
        printf("PASS\n\r");
    }
    else
    {
        printf("FAIL\n\r");
    }
    printf("\n\r");

    return 0;
}


// Get Hamming code
uint16_t enHamming(uint8_t data)
{
    uint16_t hamming = 0;

    // Set Data
    hamming = (data & BLOCK2) << BLOCK2_SHIFT;  //0b1111 0000 0000
    hamming |= (data & BLOCK1) << BLOCK1_SHIFT;  //0b0000 0111 0000
    hamming |= (data & BLOCK0) << BLOCK0_SHIFT;  //0b0000 0000 0100

    // Set Parity
    hamming |= getEvenParityforTx(data & P3_POSI_8) << P3_BIT; // 0b1111 0000
    hamming |= getEvenParityforTx(data & P2_POSI_8) << P2_BIT; // 0b1000 1110
    hamming |= getEvenParityforTx(data & P1_POSI_8) << P1_BIT; // 0b0110 1101
    hamming |= getEvenParityforTx(data & P0_POSI_8) << P0_BIT; // 0b0101 1011

    return hamming;
}

uint8_t getEvenParityforTx(uint8_t data) //Â¦¼ö Æä·¯Æ¼ 
{
    uint8_t parity = 0;
    for (int i = 0; i < 8; i++)
    {
        parity ^= (data >> i) & 0x01;
    }

    return !parity;
}

uint8_t findHammingError(uint16_t hamming)
{
    uint8_t errCode = 0;

    errCode  = ((getEvenParityforRx(hamming & P3_POSI_12)) != ((hamming >> P3_BIT) & 0x0001)) << 3;
    errCode |= ((getEvenParityforRx(hamming & P2_POSI_12)) != ((hamming >> P2_BIT) & 0x0001)) << 2;
    errCode |= ((getEvenParityforRx(hamming & P1_POSI_12)) != ((hamming >> P1_BIT) & 0x0001)) << 1;
    errCode |= ((getEvenParityforRx(hamming & P0_POSI_12)) != ((hamming >> P0_BIT) & 0x0001)) << 0;

    return errCode;
}

uint8_t getEvenParityforRx(uint16_t data) //Â¦¼ö Æä·¯Æ¼ 
{
    uint8_t parity = 0;
    for (int i = 0; i < 12; i++)
    {
        parity ^= (data >> i) & 0x01;
    }

    return !parity;
}

uint16_t fixError(uint16_t hammingData, uint8_t errorCode)
{
    if (errorCode == NONE_ERROR)
        return hammingData;

    return hammingData ^ (1 << (errorCode - 1));
}

uint8_t deHamming(uint16_t hammingData)
{
    uint8_t data = 0;

    data = (hammingData >> BLOCK2_SHIFT) & BLOCK2;
    data |= (hammingData >> BLOCK1_SHIFT) & BLOCK1;
    data |= (hammingData >> BLOCK0_SHIFT) & BLOCK0;

    return data;
}