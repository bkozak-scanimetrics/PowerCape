/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
#ifndef BOARD_WATCHDOG_H_
#define BOARD_WATCHDOG_H_
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
void board_watchdog_activity_done(void);
void board_watchdog_activity_start(void);
void board_watchdog_check(void);
/*****************************************************************************/
#endif /* BOARD_WATCHDOG_H_ */