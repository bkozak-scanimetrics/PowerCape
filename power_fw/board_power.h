/******************************************************************************
* Copyright (C) 2016, Scanimetrics Inc - All Rights Reserved                  *
*                                                                             *
* Unauthorized copying of this file, via any medium is strictly prohibited.   *
* Proprietary and confidential.                                               *
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
void board_power_req_cycle(void);
void board_power_req_powerdown(void);
void board_power_down(void);
void board_power_event(uint8_t reason);
void board_power_sm(void);
/*****************************************************************************/
#endif /* BOARD_POWER_H_ */