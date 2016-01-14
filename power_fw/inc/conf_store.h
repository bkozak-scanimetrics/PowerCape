/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
#ifndef CONF_STORE_H_
#define CONF_STORE_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdint.h>
#include <stdlib.h>

#include "twi_slave.h"
#include "registers.h"
#include "monitor.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define CONF_STORE_EEPROM_START ((uint8_t*)1)

#define CONF_STORE_DEFAULTS \
    {\
        0x00, BOARD_TYPE_BONE, (uint8_t)'A', (uint8_t)'2', TWI_SLAVE_ADDRESS, \
        0X01, 0X03, MONITOR_CONF_DEFAULT, MONITOR_BOOT_MINIUTES_DEFAULT \
    }

#define _CONF_FUNCTIONS(n, p) \
    static inline uint8_t conf_store_get_##n(void) \
        {return read_config(p);} \
    static inline void conf_store_set_##n(uint8_t v) \
        {store_config(p, v);}
/******************************************************************************
*                                    ENUMS                                    *
******************************************************************************/
enum conf_params {
    CONF_CALIBRATION,
    CONF_BOARD,
    CONF_REVISION,
    CONF_STEPPING,
    CONF_I2C_ADDR,
    CONF_CHG_CURRENT,
    CONF_CHG_TIMER,
    CONF_MONITOR_CTL,
    CONF_BOOT_MINUTES,

    CONF_COUNT
};
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
void store_config(size_t param, uint8_t value);
uint8_t read_config(size_t param);
void conf_store_task(void);
void init_conf_store(void);
/******************************************************************************
*                              INLINE FUNCTIONS                               *
******************************************************************************/
_CONF_FUNCTIONS(calibration_value, CONF_CALIBRATION)
_CONF_FUNCTIONS(board_type, CONF_BOARD)
_CONF_FUNCTIONS(revision_value, CONF_REVISION)
_CONF_FUNCTIONS(stepping_value, CONF_STEPPING)
_CONF_FUNCTIONS(i2c_address, CONF_I2C_ADDR)
_CONF_FUNCTIONS(charge_current, CONF_CHG_CURRENT)
_CONF_FUNCTIONS(charge_timer, CONF_CHG_TIMER)
_CONF_FUNCTIONS(monitor_ctl, CONF_MONITOR_CTL)
_CONF_FUNCTIONS(boot_minutes, CONF_BOOT_MINUTES)
/*****************************************************************************/
#endif /* CONF_STORE_H_ */