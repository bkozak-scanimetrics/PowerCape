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
#ifndef DEBUG_H_
#define DEBUG_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "board.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef DEBUG_SYS
#define DEBUG_SYS 0
#endif

#ifndef DEBUG_SYS_TIME
#define DEBUG_SYS_TIME 0
#endif

#ifndef DEBUG_MAIN
#define DEBUG_MAIN 0
#endif

#ifndef DEBUG_POWER
#define DEBUG_POWER 0
#endif
/******************************************************************************
*                              INLINE FUNCTIONS                               *
******************************************************************************/
#if DEBUG
static inline void debug_led_code(int c)
{
	switch(c)
	{
	case 0:
		board_led1(0);
		board_led2(0);
		break;
	case 1:
		board_led1(1);
		board_led2(0);
		break;
	case 2:
		board_led1(0);
		board_led2(1);
		break;
	case 3:
		board_led1(1);
		board_led2(1);
		break;
	default:
		break;
	}
}
/*****************************************************************************/
static inline void debug_led_count(void)
{
	int code = (board_led2_val() << 1) | (board_led1_val() << 0);

	code = (code + 1) & 0x3;

	debug_led_code(code);
}
/*****************************************************************************/
#else
#define debug_led_code(c) do{}while(0)
#define debug_led_count() do{}while(0)
#endif

#if DEBUG_SYS
#define debug_led_sys debug_led_code
#else
#define debug_led_sys(c) do{}while(0)
#endif

#if DEBUG_SYS_TIME
#define debug_led_sys_time debug_led_code
#else
#define debug_led_sys_time(c) do{}while(0)
#endif

#if DEBUG_MAIN
#define debug_count_main debug_led_count
#else
#define debug_count_main() do{}while(0)
#endif

#if DEBUG_POWER
#define debug_led_power debug_led_code
#else
#define debug_led_power(c) do{}while(0)
#endif
/*****************************************************************************/
#endif /* DEBUG_H_ */