/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "board_power.h"

#include <stdint.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "board.h"
#include "registers.h"
#include "twi_slave.h"
#include "board_watchdog.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define POWERUP_RETRIES 3
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
enum power_state_type {
    STATE_INIT,
    STATE_SETUP_OFF,
    STATE_OFF_NO_PGOOD,
    STATE_OFF_WITH_PGOOD,
    STATE_POWER_UP,
    STATE_CHECK_3V,
    STATE_ON,
    STATE_POWER_DOWN,
    STATE_CYCLE_POWER,
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint8_t power_state = STATE_INIT;
static volatile uint8_t retries;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static bool reason_valid(uint8_t reason);
static void state_powerup(void);
static void state_machine(void);
static void perform_poweron(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
/*****************************************************************************/
static bool reason_valid(uint8_t reason)
{
    return registers_get( REG_START_ENABLE ) & reason;
}
/*****************************************************************************/
static void state_powerup(void)
{
    retries = POWERUP_RETRIES;
    power_state = STATE_POWER_UP;
}
/*****************************************************************************/
static void perform_poweron(void)
{
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
        board_hold_reset();
        board_poweron();
         _delay_ms( 250 );
        board_release_reset();
    }
}
/*****************************************************************************/
static void state_machine(void)
{
    switch(power_state)
    {
    case STATE_INIT:
        if ( board_3v3() ) {
            retries = POWERUP_RETRIES;
            power_state = STATE_CHECK_3V;
        } else {
            state_powerup();
        }
        break;

    case STATE_SETUP_OFF:

        board_off_setup();

        if ( board_pgood() ) {
            power_state = STATE_OFF_WITH_PGOOD;
        } else {
            power_state = STATE_OFF_NO_PGOOD;
        }

        break;

    case STATE_OFF_NO_PGOOD:
        if ( board_pgood() ) {
            power_state = STATE_OFF_WITH_PGOOD;
            board_power_event( START_PWRGOOD );
        }
        break;

    case STATE_OFF_WITH_PGOOD:
        if ( !board_pgood() ) {
            power_state = STATE_OFF_NO_PGOOD;
        }
        break;

    case STATE_POWER_UP:
        retries--;
        power_state = STATE_CHECK_3V;

        /* note - perform_poweron enables interrupts temporarily */
        perform_poweron();
        break;

    case STATE_CHECK_3V:
        if ( board_3v3() ) {
            board_preboot_setup();
            power_state = STATE_ON;
        } else {
            board_poweroff();
            if ( retries > 0 ) {
                power_state = STATE_POWER_UP;
            }
            else {
                power_state = STATE_POWER_DOWN;
            }
        }
        break;

    case STATE_ON:
        if ( board_3v3() == 0 ) {
            power_state = STATE_POWER_DOWN;
        }
        break;

    case STATE_POWER_DOWN:
        twi_slave_stop();
        board_poweroff();
        power_state = STATE_SETUP_OFF;
        break;

    case STATE_CYCLE_POWER:
        twi_slave_stop();
        board_poweroff();

        state_powerup();
        break;
    }
}
/*****************************************************************************/
/**
* \brief return true if the board's power state is off
*
* Note that the state may be neither off nor on (such as when the board is
* being powered up for example)
**/
bool board_power_state_is_off(void)
{
    enum power_state_type state = power_state;
    return (state == STATE_OFF_NO_PGOOD) || (state == STATE_OFF_WITH_PGOOD);
}
/*****************************************************************************/
/**
* \brief return true if the board's power state is on
*
* Note that the state may be neither off nor on (such as when the board is
* being powered up for example)
**/
bool board_power_state_is_on(void)
{
    return power_state == STATE_ON;
}
/*****************************************************************************/
/**
* \brief request that the board be power cycled
**/
void board_power_req_cycle(void)
{
   power_state = STATE_CYCLE_POWER;
}
/*****************************************************************************/
/**
* \brief request that the board be powered down
**/
void board_power_req_powerdown(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if ( power_state == STATE_ON ) {
            power_state = STATE_POWER_DOWN;
        }
    }
}
/*****************************************************************************/
/**
* \brief process an event which may possible require powerup of the board
*
* \param reason bitmask describing the event which occured
**/
void board_power_event(uint8_t reason)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(board_power_state_is_off() && reason_valid(reason)) {
            state_powerup();
        }
    }
}
/*****************************************************************************/
/**
* \brief state machine for managing the board's power
**/
void board_power_sm(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        state_machine();
    }
}