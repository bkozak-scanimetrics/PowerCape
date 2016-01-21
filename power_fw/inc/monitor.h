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
#ifndef MONITOR_H_
#define MONITOR_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include "registers.h"
/******************************************************************************
*                                   DEFINES                                   *
******************************************************************************/
#define MONITOR_CONF_DEFAULT          (MONITOR_POWER_ALWAYS)
/* Note, actual boot minutes is 1 + register value */
#define MONITOR_BOOT_MINUTES_DEFAULT 1
#define MONITOR_HALT_MINUTES_DEFAULT 1
#define MONITOR_RUN_SECONDS_DEFAULT   60
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
void moinitor_poweroff(void);
void mointor_poweron(void);
void monitor_activity(void);
void monitor_state_machine(void);
/*****************************************************************************/
#endif /* MONITOR_H_ */