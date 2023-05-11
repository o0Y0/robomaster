//#include "UI.h"
#include "bsp_pm01_api.h"
#include "cmsis_os.h"


void uiTask(void const * argument)
{
	TickType_t systick = 0;

    while (1)
    {
		systick = osKernelSysTick();

        /* code */
//        Startjudge_task();  //UI
		
		
		vTaskDelay(1);
//        osDelayUntil(&systick, 1);//¾ø¶ÔÑÓÊ±
    }
}


