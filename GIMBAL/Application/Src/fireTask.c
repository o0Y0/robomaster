#include "fireTask.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "bsp_rc.h"
#include "pid.h"

#define USE_REFEREE 0  //裁判

GUN gun=
{
    .working=0,
    .mode=0, 

    .frictionWheelSpeed=
    {
        [_0M]=0,
        [_15M]=4450,
        [_18M]=4900,
		[_22M]=5500,
        [_30M]=7100,
    },
    .fricL=&DJI_Motor[SHOOT_L],
    .fricR=&DJI_Motor[SHOOT_R],

    .triggerBuf=7,
    .trigger=&DJI_Motor[TRIGGER],
};


//shoot 待修改
void fireTask(void const * argument)
{
//                   死区 积分限幅 总限幅 p   i  d 
	float fricL_pid[6]={0, 3000,  15000, 13,0.f,0},//i 0.1f
		  fricR_pid[6]={0, 3000,  15000, 13,0.f,0},//  0.1
		trigger_pid[6]={0, 1200,12000,9.f,0.2f,0};// 0.24
		
		PID_Init(&gun.fricL->pid,fricL_pid);
		PID_Init(&gun.fricR->pid,fricR_pid);
		PID_Init(&gun.trigger->pid,trigger_pid);
    
    TickType_t systick = 0;

    while (1)
    {
        systick = osKernelSysTick();
        /* code */
        if(gun.working)
        {

			if(ABS(gun.fricR->Data.velocity)>3000||ABS(gun.fricL->Data.velocity)>3000)
			{
				fire_ctrl();
			}
			
            switch (mcu_CAN.chassis.referee.shoot_expectSpeed +(1-USE_REFEREE)*15)
            {
            case 15:
                gun.bulletSpeed = _15M;
                break;
            case 18:
                gun.bulletSpeed = _18M;
                break;
            case 22:
                gun.bulletSpeed = _22M;
                break;
            case 30:
                gun.bulletSpeed = _30M;
                break;
            default:
                break;
            }

        }
        else
        {
            gun.bulletSpeed = _0M;
            gun.mode = 0;
        }

        VAL_Limit(gun.frictionWheelSpeed[gun.bulletSpeed],0,7300);
        
        f_PID_Calculate(&gun.fricL->pid,-gun.frictionWheelSpeed[gun.bulletSpeed],gun.fricL->Data.velocity);
        f_PID_Calculate(&gun.fricR->pid, gun.frictionWheelSpeed[gun.bulletSpeed],gun.fricR->Data.velocity);

		vTaskDelay(1);
//        osDelay(1);
    }
}

//开火模式切换
void fire_ctrl(void)
{
    static float triggerAngle;
    static uint8_t flag_mouse_left;
	
	if(!USE_REFEREE) gun.mode =1;

    //点射模式
    if(rc_ctrl.mouse.press_l==0&&rc_ctrl.rc.ch[3]<=630){flag_mouse_left = 0;}
    if(flag_mouse_left==0&&gun.mode==0&&(rc_ctrl.mouse.press_l||(rc_ctrl.rc.ch[3]==660)))
    {
        flag_mouse_left = 1;
#if USE_REFEREE
    if(mcu_CAN.chassis.referee.shoot_remainingHeat>15)
    {
#endif
        triggerAngle = gun.trigger->Data.angle
                        - 360.0f/gun.triggerBuf;
        if(triggerAngle<=0)triggerAngle+=360.0f;
    }
#if USE_REFEREE
    }
#endif

    float angle_Err,speed_Err;

//角度误差计算
    angle_Err = triggerAngle - gun.trigger->Data.angle;
//角度误差处理   
    if(angle_Err<-180.0f)angle_Err+=360.0f;
    if(angle_Err>=180.0f)angle_Err-=360.0f;
//速度误差计算
    speed_Err = angle_Err*57.29578f - gun.trigger->Data.velocity;

    //连发模式
    if(gun.mode==1)
    {
        triggerAngle = gun.trigger->Data.angle;
        //速度误差计算
        speed_Err = - rc_ctrl.mouse.press_l * 5000
                    - gun.trigger->Data.velocity
				    - rc_ctrl.rc.ch[3]*8;
        if(mcu_CAN.chassis.referee.shoot_remainingHeat<46 && USE_REFEREE)
        {
            gun.mode = 0;
        }
    }

//速度控制(PID控制方案)
    f_PID_Calculate(&gun.trigger->pid,speed_Err,0);
}


