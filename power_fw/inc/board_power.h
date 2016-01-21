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
#ifndef BOARD_POWER_H_
#define BOARD_POWER_H_
/******************************************************************************
*                                  INCLUDES                                   *
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************
*                             FUNCTION PROTOTYPES                             *
******************************************************************************/
bool board_power_state_is_on(void);
bool board_power_state_is_off(void);
bool board_power_state_pgood(void);
void board_power_req_cycle(uint8_t reason);
void board_power_req_powerdown(void);
void board_power_down(void);
void board_power_event(uint8_t reason);
void board_power_sm(void);
/*****************************************************************************/
#endif /* BOARD_POWER_H_ */