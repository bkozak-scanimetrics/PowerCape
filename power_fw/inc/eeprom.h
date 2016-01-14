#ifndef __EEPROM_H__
#define __EEPROM_H__

#define EEPROM_FLAGS        ( (uint8_t*)0 )

#define EE_FLAG_LOADER      0x01

void eeprom_set_bootloader_flag( void );

#endif  // __EEPROM_H__
