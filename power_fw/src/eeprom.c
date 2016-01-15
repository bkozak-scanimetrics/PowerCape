/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "eeprom.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/eeprom.h>
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
void eeprom_set_bootloader_flag( void )
{
    uint8_t i;

    i = eeprom_read_byte( EEPROM_FLAGS );
    i |= EE_FLAG_LOADER;
    eeprom_update_byte( EEPROM_FLAGS, i );
    eeprom_busy_wait();
}
/*****************************************************************************/
