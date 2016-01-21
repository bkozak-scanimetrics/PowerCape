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
#else
#define debug_led_code(c)
#endif
/*****************************************************************************/
#endif /* DEBUG_H_ */