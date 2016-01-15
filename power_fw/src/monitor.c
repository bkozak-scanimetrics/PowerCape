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
    MONITOR_WAIT_BOOT,
    MONITOR_WAIT_HALT
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint32_t activity_time;
static volatile enum monitor_state state;

static uint32_t restore_time;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static bool restoration_check(void);
static bool activity_expired(uint32_t time);
static void update_activity_time(void);
static bool run_failure(void);
static void change_state(enum monitor_state s);
static void cmd_reset(void);
static bool halt_requested(void);
static bool boot_failure(void);
static void clear_restore_regs(void);
static void setup_restore_time(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static void clear_restore_regs(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        registers_set(REG_RESTART_SECONDS, 0);
        registers_set(REG_RESTART_MINUTES, 0);
        registers_set(REG_RESTART_HOURS, 0);
    }
}
/*****************************************************************************/
static void setup_restore_time(void)
{
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;

    if(!(registers_get(REG_MONITOR_CTL) & MONITOR_TIMED_RESTORE)) {
        restore_time = 0;
        clear_restore_regs();
        return;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        seconds = registers_get(REG_RESTART_SECONDS);
        minutes = registers_get(REG_RESTART_MINUTES);
        hours   = registers_get(REG_RESTART_HOURS);

        clear_restore_regs();
    }

    restore_time  = seconds * 1;
    restore_time += minutes * 60;
    restore_time += hours   * 3600;
}
/*****************************************************************************/
static void cmd_reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(registers_get(REG_MONITOR_CMD)) {
            registers_set(REG_MONITOR_CMD, MONITOR_CMD_ERR);
        }
    }
}
/*****************************************************************************/
static bool halt_requested(void)
{
    bool ret = false;

    uint8_t cmd;
    uint8_t code;

    if(registers_get(REG_MONITOR_CTL) & MONITOR_HALT) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

            cmd = registers_get(REG_MONITOR_CMD);
            ret = (cmd == MONITOR_CMD_HALT);

            code = (ret || !cmd) ? MONITOR_CMD_OK : MONITOR_CMD_ERR;
            registers_set(REG_MONITOR_CMD, code);
        }
    } else {
        cmd_reset();
    }

    return ret;
}
/*****************************************************************************/
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
    if(restore_time){
        return activity_expired(restore_time) &&
               activity_expired(POWER_RESTORE_SECONDS);
    } else if(registers_get(REG_MONITOR_CTL) & MONITOR_POWER_ALWAYS) {
        return activity_expired(POWER_RESTORE_SECONDS);
    } else {
        return false;
    }
}
/*****************************************************************************/
static bool halt_failure(void)
{
    uint32_t timeout = (1 + registers_get(REG_MONITOR_HALT_MINUTES)) * 60U;

    return activity_expired(timeout);
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
    setup_restore_time();
    change_state(MONITOR_OFF);
}
/*****************************************************************************/
/**
* \brief call when power is regained to notify the monitor state machine
**/
void mointor_poweron(void)
{
    cmd_reset();

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

            if(registers_get(REG_MONITOR_CTL) & MONITOR_TIMED_RESTORE) {
                board_power_event(START_TIMEOUT);
            } else {
                board_power_event(START_MONITOR);
            }
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
        if(halt_requested()) {
            change_state(MONITOR_WAIT_HALT);
        } else if(run_failure()) {
            board_power_req_cycle(START_WDT);
            change_state(MONITOR_CYCLE);
        }
        break;
    case MONITOR_WAIT_HALT:
        if(halt_failure()) {
            board_power_req_powerdown();
            change_state(MONITOR_CYCLE);
        }
        break;
    }
}
/*****************************************************************************/