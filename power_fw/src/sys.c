/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "sys.h"

#include "board.h"
#include "registers.h"
#include "twi_slave.h"
#include "monitor.h"
#include "sys_time.h"
#include "board_watchdog.h"
#include "board_power.h"
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
/**
* \brief called when entering the poweroff state
*
* Everything that cares about poweroff events should have code here
**/
void sys_notify_off(void)
{
    twi_slave_stop();
    board_enable_interrupt(registers_get(REG_START_ENABLE));

    moinitor_poweroff();
}
/*****************************************************************************/
/**
* \brief called when entering the poweron state
*
* Everything that cares about poweron events should have code here
**/
void sys_notify_on(void)
{
    twi_slave_init();
    board_disable_interrupt(START_ALL);
    board_watchdog_boot_setup();

    mointor_poweron();
}
/*****************************************************************************/
/**
* \brief called when rebooting the board
*
* Note that, once this is called, we know that we will soon get either a
* sys_notify_on (if the reboot worked) or a sys_notify_off (if the reboot
* failed).
**/
void sys_notify_reboot(void)
{
}
/*****************************************************************************/
/**
* \brief called when board is off and dc power is applied
*
* Everything that cares about restoration of dc power should go here
**/
void sys_notify_off_and_pgood(void)
{
    board_power_event(START_PWRGOOD);
}
/*****************************************************************************/
/**
* \brief called when board is off and dc power is lost
*
* Everything that cares about loss of dc power should go here
**/
void sys_notify_off_and_pbad(void)
{

}
/*****************************************************************************/