/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
#ifndef SYS_TIME_H_
#define SYS_TIME_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdint.h>
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
uint8_t sys_time_begin_countdown(void);
void sys_time_init(void);
uint32_t sys_time_get_ticks(void);
uint32_t sys_time_get_time(void);
void sys_time_set_time(uint32_t time);
/*****************************************************************************/
#endif /* SYS_TIME_H_ */