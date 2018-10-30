
#include <stdio.h>
// source code from https://www.embeddedrelated.com/showcode/295.php
#include "crc-16.h"

//Tested
#define CRC16_DNP	0x3D65		// DNP, IEC 870, M-BUS, wM-BUS, ...
#define CRC16_CCITT	0x1021		// X.25, V.41, HDLC FCS, Bluetooth, ...

//Other polynoms not tested
#define CRC16_IBM	0x8005		// ModBus, USB, Bisync, CRC-16, CRC-16-ANSI, ...
#define CRC16_T10_DIF	0x8BB7		// SCSI DIF
#define CRC16_DECT	0x0589		// Cordeless Telephones
#define CRC16_ARINC	0xA02B		// ACARS Aplications

#define POLYNOM		CRC16_CCITT   // Define the used polynom from one of the aboves



static unsigned int crc16(unsigned int crcValue, unsigned char newByte) 
{
	unsigned char i;

	for (i = 0; i < 8; i++) {

		if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)){
			crcValue = (crcValue << 1)  ^ POLYNOM;
		}else{
			crcValue = (crcValue << 1);
		}

		newByte <<= 1;
	}
  
	return crcValue;
}



unsigned short calc_crc16(char *Data, unsigned  length )
{

	unsigned crc;
	int aux = 0;
    unsigned temp;
	
	crc = 0xFFFF; //Initialization of crc to 0xFFFF for CCITT

	while (aux < length){
		crc = crc16(crc,Data[aux]);
		aux++;
	}

	temp = crc;
    //printf("crc is %x \n",crc);
    crc = (temp&0xff)<<8;
    //printf("crc is %x \n",crc);
    crc = crc | ((temp>>8)&0xff);
    //printf("crc is %x \n",crc);
	return crc; //The crc value for CCITT
}
