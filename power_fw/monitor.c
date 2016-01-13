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
	MONITOR_WAIT,
	MONITOR_OFF,
	MONITOR_ON
};
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static uint32_t loss_time;
static enum monitor_state state;
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
static bool restoration_check(void);
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
static bool restoration_check(void)
{
	return (sys_time_get_ticks() - loss_time) >= POWER_RESTORE_SECONDS;
}
/*****************************************************************************/
/**
* \brief call when power is lost to notify the monitor state machine
**/
void moinitor_poweroff(void)
{
	loss_time = sys_time_get_ticks();
	state = MONITOR_OFF;
}
/*****************************************************************************/
/**
* \brief call when power is regained to notify the monitor state machine
**/
void mointor_poweron(void)
{
	state = MONITOR_ON;
}
/*****************************************************************************/
/**
* \brief runs the monitor state machine
*
* to be called once every system tick
**/
void monitor_state_machine(void)
{
	switch(state) {
	case MONITOR_WAIT:
		break;
	case MONITOR_OFF:
		if(restoration_check()) {
			board_power_event(START_PWRGOOD);
			state = MONITOR_WAIT;
		}
		break;
	case MONITOR_ON:
		break;
	}
}
/*****************************************************************************/