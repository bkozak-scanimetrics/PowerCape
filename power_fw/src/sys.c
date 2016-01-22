/******************************************************************************
* Copyright (c) 2016 Scanimetrics Inc.                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy                                                                        *
* of this software and associated documentation files (the "Software"), to    *
* deal                                                                        *
* in the Software without restriction, including without limitation the       *
* rights                                                                      *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell   *
* copies of the Software, and to permit persons to whom the Software is       *
* furnished to do so, subject to the following conditions:                    *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
******************************************************************************/
/**
* @author Billy Kozak <bkozak@scanimetrics.com>
**/
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "sys.h"

#include "board.h"
#include "registers.h"
#include "twi_slave.h"
#include "monitor.h"
#include "sys_time.h"
#include "board_power.h"
#include "debug.h"
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
	debug_led_sys(1);

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
	debug_led_sys(2);

	twi_slave_init();
	board_disable_interrupt(START_ALL);

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
	twi_slave_stop();
	debug_led_sys(3);
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
