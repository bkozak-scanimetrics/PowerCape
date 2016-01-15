/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
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