# pragma once


/*
Standard Message Format:
---
[0] 1 Byte: Start byte (0xa0)
[1] 2 Byte: Sample Number
[2-3] 3-4 Bytes: EXG channel 1
[4-5] 5-6 Bytes: Data value for EXG channel 2
[6-7] 7-8 Bytes: Data value for EXG channel 3
[8-9] 9-10 Bytes: Data value for EXG channel 4
[10-11] 11-12 Bytes: Data value for EXG channel 5
[12-13] 13-14 Bytes: Data value for EXG channel 6
[14-15] 15-16 Bytes: Data value for EXG channel 7
[16-17] 17-18 Bytes: Data value for EXG channel 8
[18] 19 Byte: Data LOFF STATP
[19] 20 Byte: Data LOFF STATN
[20] 21 Byte: End byte (0xc0)

IMU Message Format:
---
Frame format (Arduino):
[0]    1 Byte : START (0xA0)
[1]    1 Byte : counter
[2..17]16 Bytes: 8x EXG int16 (little-endian)
[18]   1 Byte : LOFF STATP
[19]   1 Byte : LOFF STATN
[20..55]36 Bytes: 9x float32 IMU, little-endian: ax,ay,az,gx,gy,gz,mx,my,mz
[56]   1 Byte : END (0xC0)
*/
namespace KnightProtocolConstants
{
    const unsigned char START_BYTE = 0xa0;
    const unsigned char END_BYTE = 0xc0;
    const unsigned char MESSAGE_LENGTH = 21;
    const unsigned char IMU_MESSAGE_LENGTH = 57;

    const unsigned char EXG_DATA_OFFSET = 2;
    const unsigned char LOffStatP_DATA_OFFSET = 18;
    const unsigned char LOffStatN_DATA_OFFSET = 19;
    const unsigned char IMU_DATA_OFFSET = 20;
    
    const unsigned char CHANNEL_COUNT = 8;
    const unsigned char SAMPLE_RATE = 125;
    const unsigned int BAUD_RATE = 115200;
    const double BASE_EEG_SCALE = 1000000.0 * 4.0 / 0x7fff;
}