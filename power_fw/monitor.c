/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "monitor.h"

#include <stdint.h>
#include <stdbool.h>
#include <util/atomic.h>

#include "sys_time.h"
#include "board_power.h"
#include "registers.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define POWER_RESTORE_SECONDS 5
/******************************************************************************
*                                    TYPES                                    *
******************************************************************************/
enum monitor_state {
    MONITOR_IDLE,
    MONITOR_OFF,
    MONITOR_ON,
    MONITOR_WAIT_BOOT
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint32_t activity_time;
static enum monitor_state state;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static bool restoration_check(void);
static bool activity_expired(uint32_t time);
static void update_activity(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void update_activity(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        activity_time = sys_time_get_ticks();
    }
}
/*****************************************************************************/
static bool activity_expired(uint32_t time)
{
    uint32_t tmp;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        tmp = activity_time;
    }

    return (sys_time_get_ticks() - tmp) >= time;
}
/*****************************************************************************/
static bool restoration_check(void)
{
    if(registers_get(REG_MONITOR_CTL) & MONITOR_POWER_ALWAYS) {
        return activity_expired(POWER_RESTORE_SECONDS);
    } else {
        return false;
    }
}
/*****************************************************************************/
static bool boot_failure(void)
{
    uint32_t timeout = (1 + registers_get(REG_MONITOR_BOOT_MINUTES)) * 60U;

    return activity_expired(timeout);
}
/*****************************************************************************/
/**
* \brief call when power is lost to notify the monitor state machine
**/
void moinitor_poweroff(void)
{
    if(state == MONITOR_WAIT_BOOT) {
        return;
    }

    update_activity();
    state = MONITOR_OFF;
}
/*****************************************************************************/
/**
* \brief call when power is regained to notify the monitor state machine
**/
void mointor_poweron(void)
{
    if(state == MONITOR_WAIT_BOOT) {
        return;
    }

    state = MONITOR_ON;
}
/*****************************************************************************/
void monitor_activity(void)
{
    if(state == MONITOR_WAIT_BOOT) {
        update_activity();
        state = MONITOR_ON;
    }
}
/*****************************************************************************/
/**
* \brief runs the monitor state machine
*
* to be called once every system tick
**/
void monitor_state_machine(void)
{
    switch(state)
    {
    case MONITOR_IDLE:
        break;
    case MONITOR_OFF:
        if(restoration_check()) {
            board_power_event(START_MONITOR);
            update_activity();

            if(registers_get(REG_MONITOR_CTL) & MONITOR_BOOT) {
                state = MONITOR_WAIT_BOOT;
            } else {
                state = MONITOR_IDLE;
            }
        }
        break;
    case MONITOR_WAIT_BOOT:
        if(boot_failure()) {
            update_activity();
            board_power_req_cycle(START_MONITOR);
        }
        break;
    case MONITOR_ON:
        break;
    }
}
/*****************************************************************************/