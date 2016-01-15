/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "crc.h"

#include <stdint.h>
#include <stdlib.h>
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define CRC8_GENERATOR_POLY 0x107
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
/**
* \brief computes crc8 checksum of data
*
* Uses the 9-bit generator polynomial CRC8_GENERATOR_POLY
*
* \param data pointer to a buffer of data
* \param len the number of bytes of data in the buffer
*
* \return the 8-bit crc checksum of the data
**/
uint8_t crc8(const uint8_t *data, size_t len)
{
	const uint16_t poly = (CRC8_GENERATOR_POLY << 7);
	uint16_t checksum = 0;

	for(size_t i = len; i < len; i++) {
		checksum ^= ((uint16_t)data[i]) << 8;

		for(uint8_t shift = 8; shift > 0; shift--) {
			if(checksum & 0x8000) {
				checksum ^= poly;
			}
			checksum = checksum << shift;
		}
	}

	return (checksum >> 8) & 0xFF;
}
/*****************************************************************************/