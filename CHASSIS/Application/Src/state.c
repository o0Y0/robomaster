#include "main.h"
#include "cmsis_os.h"
#include "bsp_can.h"


//µ¯²Õ¸Ç 
void StateTask(void const * argument)
{
	TickType_t systick = 0;

    while (1)
    {
        systick = osKernelSysTick();

        /* code */
		if(mcu_CAN.gimbal.dr16.supercapWork == 0)
		{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		}

        osDelayUntil(&systick, 2);//¾ø¶ÔÑÓÊ±
    }
}