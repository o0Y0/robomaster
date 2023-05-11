#include "gimbalTask.h"
#include "cmsis_os.h"
#include "bsp_rc.h"
#include "tim.h"

/*--------------------------   变量声明   ------------------------------*/
uint8_t coverSwitch = 0;
/*--------------------------   变量声明   ------------------------------*/




//弹舱盖_待修改
void coverTask(void const * argument)
{
//    TickType_t systick = 0;

    while (1)
    {
//       systick = osKernelSysTick();

        /* code */
        static uint8_t flag_r = 0;
        if(rc_ctrl.key.set.R &&flag_r==0)
        {
            flag_r = 1;
            coverSwitch = !coverSwitch;
        }
        if(rc_ctrl.key.set.R ==0)
        {
            flag_r =  0;
        }
        if(coverSwitch){
            gimbal.pitMode = 1;
			
//改			
//            gimbal.pitTarget[1] = 120.f;
//           if(ABS(gimbal.pitReal[1]-gimbal.pitTarget[1])<3.f)
//            {
			
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,1000);
			
 //           }
        }
        if(!coverSwitch)
        {
            gimbal.pitMode = 0;
			
			
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,3400);
			
			
        }

		vTaskDelay(2);
//        osDelay(2);
    }
}
