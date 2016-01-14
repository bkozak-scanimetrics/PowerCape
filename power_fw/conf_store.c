/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "conf_store.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "crc.h"
#include "board.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define LOAD_RETRIES   2
#define RETRY_DELAY_MS 250
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static const uint8_t defaults[CONF_COUNT] = CONF_STORE_DEFAULTS;
static uint8_t conf_buffer[CONF_COUNT + 1];

static bool conf_dirty;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static void load_rom(void);
static bool checksum_ok(void);
static void load_defaults(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void load_rom(void)
{
    eeprom_read_block(conf_buffer, CONF_STORE_EEPROM_START, CONF_COUNT + 1);
}
/*****************************************************************************/
static bool checksum_ok(void)
{
    return crc8(conf_buffer, CONF_COUNT) == conf_buffer[CONF_COUNT];
}
/*****************************************************************************/
static void load_defaults(void)
{
    memcpy(conf_buffer, defaults, CONF_COUNT);
}
/*****************************************************************************/
/**
* \brief stores a parameter in the config store
**/
void store_config(size_t param, uint8_t value)
{
    conf_dirty = true;
    conf_buffer[param] = value;
}
/*****************************************************************************/
/**
* \brief reads a parameter from the config store
**/
uint8_t read_config(size_t param)
{
    return conf_buffer[param];
}
/*****************************************************************************/
/**
* \brief performs config store related tasks
*
* the purpose of this task is to perform tasks which take too long to be put
* directly into an interrupt routine.
**/
void conf_store_task(void)
{
    uint8_t *store_addr    = CONF_STORE_EEPROM_START;
    uint8_t *checksum_addr = store_addr + CONF_COUNT;

    bool dirty;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        dirty = conf_dirty;
        conf_dirty = false;
    }

    if(!dirty) {
        return;
    }

    eeprom_write_block(conf_buffer, store_addr, CONF_COUNT);
    eeprom_write_byte(checksum_addr, crc8(conf_buffer, CONF_COUNT));
}
/*****************************************************************************/
/**
* \brief initilize the configuration store
**/
void init_conf_store(void)
{
    bool conf_ok = false;

    for(int i = 0; i < LOAD_RETRIES; i++) {
        load_rom();
        conf_ok = checksum_ok();
        if(conf_ok) {
            break;
        }
         _delay_ms(RETRY_DELAY_MS);
    }

    if(!conf_ok) {
        load_defaults();
        conf_dirty = true;
        conf_store_task();
    }
}
/*****************************************************************************/