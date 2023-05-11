//
// Created by YanYuanbin on 22-10-3.
//
#include "bsp_tim.h"

#include "tim.h"

/**
  * @brief  Starts the PWM signal generation.
  * @param  None
  * @retval None
  */
void bsp_tim_init(void)
{
	//Heat_Power_Tim init
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
}

/**
  * @brief  Bmi088 Heat Power PWM Control
  * @param  None
  * @retval None
  */
void Heat_Power_Ctl(uint16_t tempPWM)
{
	  __HAL_TIM_SetCompare(&htim10, TIM_CHANNEL_1, tempPWM);
}
