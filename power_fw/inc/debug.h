/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
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