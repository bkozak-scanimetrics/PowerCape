/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "board_watchdog.h"

#include <stdint.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "registers.h"
#include "board.h"
#include "board_power.h"
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint8_t activity_watchdog;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static void watchdog_reset(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void watchdog_reset(void)
{
    // Make sure there is no start reason
    registers_set( REG_START_REASON, 0 );
    board_hold_reset();
    _delay_ms( 250 );
    board_release_reset();
}
/*****************************************************************************/
/**
* \brief call when activity from host has been detected
**/
void board_watchdog_activity(void)
{
    activity_watchdog = 0;
}
/*****************************************************************************/
/**
* \brief call to start the host activity watchdog
**/
void board_watchdog_boot_setup(void)
{
    registers_set( REG_WDT_RESET, 0);
    registers_set( REG_WDT_POWER, 0);
    registers_set( REG_WDT_STOP, 0);
    activity_watchdog = registers_get(REG_WDT_START);
}
/*****************************************************************************/
/**
* \brief check for watchdog triggering conditions & countdown ticks
*
* Should be called only once per system tick
**/
void board_watchdog_check(void)
{
    uint8_t i;

    // Check reset watchdog
    i = registers_get( REG_WDT_RESET );
    if ( i != 0 ) {
        i -= 1;
        registers_set( REG_WDT_RESET, i );
        if ( i == 0 ) {
            watchdog_reset();
            registers_set( REG_WDT_POWER, 0 );
            registers_set( REG_WDT_STOP, 0 );
        }
    }

    // Check power-cycle watchdog
    i = registers_get( REG_WDT_POWER );
    if ( i != 0 ) {
        i -= 1;
        registers_set( REG_WDT_POWER, i );
        if ( i == 0 ) {
            board_power_req_cycle(START_WDT);
        }
    }

    // Check power-down watchdog
    i = registers_get( REG_WDT_STOP );
    if ( i != 0 ) {
        i -= 1;
        registers_set( REG_WDT_STOP, i );
        if ( i == 0 ) {
            board_power_req_powerdown();
        }
    }

    // Check start-up activity watchdog
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(activity_watchdog != 0) {
            activity_watchdog -= 1;
            if(activity_watchdog == 0) {
                 board_power_req_cycle(START_WDT);
            }
        }
    }
}
/*****************************************************************************/
