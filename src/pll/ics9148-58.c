/*
*  C Implementation: ics9148-58
*
* Description: ics9148-58
*
*
* Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "i2c.h"

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct
{
	unsigned char fsb;
	unsigned char ctrlb;
	float pci;
	float agp;
} PLL_t;
#define BYTECOUNT   6
#define CONTROLBYTE 0
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{ 67, 0x18, 33.40, 66.00},
	{ 75, 0x38, 37.50, 75.00},
	{ 83, 0x58, 33.33, 66.66},
	{ 95, 0x68, 31.75, 63.50},
	{100, 0x78, 33.33, 33.66},
	{112, 0x08, 37.30, 74.70},
	{124, 0x28, 41.30, 82.70},
	{133, 0x48, 44.30, 88.70},
	{0}
};

int ics9148_58_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
			buf[CONTROLBYTE] = pll[i].ctrlb;
	if(!buf[CONTROLBYTE]) return -1;

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else printf("DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<BYTECOUNT; i++) printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics9148_58_GetFSB()
{
	int i, file, res;
	unsigned char buf[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else printf("DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++) printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf[CONTROLBYTE]) return pll[i].fsb;

	return -1;
}

int ics9148_58_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			if(pci) *pci = pll[i].pci;
			if(agp) *agp = pll[i].agp;
			return 0;
		}

	return -1;
}

int ics9148_58_GetFirstFSB()
{
	FSBIndex = 0;
  if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int ics9148_58_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

