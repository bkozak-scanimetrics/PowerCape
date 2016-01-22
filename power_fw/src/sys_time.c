/******************************************************************************
* Original work Copyright (c) 2014 AndiceLabs
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
#include "sys_time.h"

#include <stdint.h>
#include <util/atomic.h>

#include "registers.h"
#include "board_power.h"
#include "board.h"
#include "registers.h"
#include "debug.h"
/******************************************************************************
*                                    DATA                                     *
******************************************************************************/
static volatile uint32_t system_ticks;
static volatile uint32_t rtc_offset;
/******************************************************************************
*                            FUNCTION DEFINITIONS                             *
******************************************************************************/
/**
* \brief initilize timer2
**/
void sys_time_init( void )
{
	PRR &= ~( 1 << PRTIM2 );    // is this necessary with async mode?
	ASSR = ( 1 << AS2 );        // external crystal

	while ( ASSR & ( 1 << TCR2BUB ) );
	TCCR2B = 0;

	while ( ASSR & ( 1 << TCR2AUB ) );
	TCCR2A = 0;

	while ( ASSR & ( 1 << TCN2UB ) );
	TCNT2 = 0;

	while ( ASSR & ( 1 << TCR2BUB ) );
	TCCR2B = ( 1 << CS22 ) | ( 1 << CS20 );    // clk/128 (1s)


	TIMSK2 = ( 1 << TOIE2 );
}
/*****************************************************************************/
/**
* \brief get the time in system ticks (in seconds)
**/
uint32_t sys_time_get_ticks(void)
{
	uint32_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ret = system_ticks;
	}

	return ret;
}
/*****************************************************************************/
/**
* \brief get the real rtc clock time (in seconds)
**/
uint32_t sys_time_get_time(void)
{
	uint32_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ret = system_ticks + rtc_offset;
	}

	return ret;
}
/*****************************************************************************/
/**
* \brief set the rtc clock time to the given time (in seconds)
**/
void sys_time_set_time(uint32_t time)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		rtc_offset = time - system_ticks;
	}
}
/*****************************************************************************/
ISR( TIMER2_OVF_vect, ISR_BLOCK )
{
	static uint8_t button_hold_count = 0;

	// Handle RTC
	system_ticks++;

	debug_led_sys_time(system_ticks & 0x3);

	// Forced power-off check
	if ( ( PIND & PIN_BUTTON ) == 0 )
	{
		button_hold_count++;
		if ( button_hold_count == 5 )
		{
			board_power_req_powerdown();
		}
	}
	else
	{
		button_hold_count = 0;
	}
}
/*****************************************************************************/
