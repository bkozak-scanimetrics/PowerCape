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