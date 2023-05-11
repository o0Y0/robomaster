//
// Created by YanYuanbin on 23-3-30.
//
#ifndef MESSAGE_H
#define MESSAGE_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported functions --------------------------------------------------------*/
extern float const *f_get_bmi088_message(void);
extern void get_CAN1_RxMessage(void);
extern void get_CAN2_RxMessage(void);

#endif //MESSAGE_H
