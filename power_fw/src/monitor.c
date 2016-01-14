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
    MONITOR_CYCLE,
    MONITOR_OFF,
    MONITOR_ON,
    MONITOR_WAIT_BOOT
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint32_t activity_time;
static volatile enum monitor_state state;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static bool restoration_check(void);
static bool activity_expired(uint32_t time);
static void update_activity_time(void);
static bool run_failure(void);
static void change_state(enum monitor_state s);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void change_state(enum monitor_state s)
{
    update_activity_time();
    state = s;
}
/*****************************************************************************/
static void update_activity_time(void)
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
static bool run_failure(void)
{
    uint32_t timeout;

    if(!(registers_get(REG_MONITOR_CTL) & MONITOR_RUN)) {
        return false;
    }

    timeout = (1 + registers_get(REG_MONITOR_RUN_SECONDS));

    return activity_expired(timeout);
}
/*****************************************************************************/
/**
* \brief call when power is lost to notify the monitor state machine
**/
void moinitor_poweroff(void)
{
    change_state(MONITOR_OFF);
}
/*****************************************************************************/
/**
* \brief call when power is regained to notify the monitor state machine
**/
void mointor_poweron(void)
{
    if(registers_get(REG_MONITOR_CTL) & MONITOR_BOOT) {
        change_state(MONITOR_WAIT_BOOT);
    } else {
        change_state(MONITOR_ON);
    }
}
/*****************************************************************************/
/**
* \brief call whenever host activity is detected
**/
void monitor_activity(void)
{
    if((state == MONITOR_WAIT_BOOT) || (state == MONITOR_ON)) {
        update_activity_time();

        if(state == MONITOR_WAIT_BOOT) {
            state = MONITOR_ON;
        }
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
    case MONITOR_CYCLE:
        /* we are undergoing a power-cycle and expect either a poweron
           or poweroff notification to happen soon. */
        break;
    case MONITOR_OFF:
        if(restoration_check()) {
            board_power_event(START_MONITOR);
            change_state(MONITOR_CYCLE);
        }
        break;
    case MONITOR_WAIT_BOOT:
        if(boot_failure()) {
            board_power_req_cycle(START_MONITOR);
            change_state(MONITOR_CYCLE);
        }
        break;
    case MONITOR_ON:
        if(run_failure()) {
            board_power_req_cycle(START_MONITOR);
            change_state(MONITOR_CYCLE);
        }
        break;
    }
}
/*****************************************************************************/