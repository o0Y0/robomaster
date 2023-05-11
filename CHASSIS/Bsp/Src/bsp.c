//
// Created by YanYuanbin on 22-10-2.
//
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "robot_ref.h"

#include "bsp_tim.h"
#include "bsp_rc.h"
#include "bsp_can.h"

#include "BMI088_Driver.h"
#include "IST8310_Driver.h"


/**
  * @brief  config board periph
  * @retval None
  */
void BSP_Init(void)
{
		/* starts the pwm signal generation. */
		bsp_tim_init();

#if defined(GIMBAL_BOARD)
		/* config remote control transfer*/
		remote_control_init();
#endif
	
		/* config the can module. transfer*/
		can_init();
		
		/* config bmi088 transfer */
		while(BMI088_init())  
		{
				HAL_Delay(100);
		}
		/* config ist8310 transfer */
//    while(ist8310_init())
//    {
//        HAL_Delay(100);
//    }
}


