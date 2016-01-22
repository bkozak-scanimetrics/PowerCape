/******************************************************************************
* Copyright (c) 2016 Scanimetrics Inc.                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy                                                                        *
* of this software and associated documentation files (the "Software"), to    *
* deal                                                                        *
* in the Software without restriction, including without limitation the       *
* rights                                                                      *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell   *
* copies of the Software, and to permit persons to whom the Software is       *
* furnished to do so, subject to the following conditions:                    *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
******************************************************************************/
/**
* @author Billy Kozak <bkozak@scanimetrics.com>
**/
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

	for(size_t i = 0; i < len; i++) {
		checksum ^= ((uint16_t)data[i]) << 8;

		for(uint8_t j = 0; j < 8; j++) {

			if(checksum & 0x8000) {
				checksum ^= poly;
			}
			checksum = checksum << 1;
		}
	}

	return (checksum >> 8) & 0xFF;
}
/*****************************************************************************/
