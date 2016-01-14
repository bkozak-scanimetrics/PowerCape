/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
******************************************************************************/
#ifndef SYS_H_
#define SYS_H_
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
void sys_notify_off(void);
void sys_notify_on(void);
void sys_notify_reboot(void);
void sys_notify_off_and_pgood(void);
void sys_notify_off_and_pbad(void);
/*****************************************************************************/
#endif /* SYS_H_ */