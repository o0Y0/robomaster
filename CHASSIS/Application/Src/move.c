#include "move.h"
#include "cmsis_os.h"
#include "arm_math.h"

#define VW_MAX 6000
#define POWER_CTRL_ENABLE 1


CHASSIS chassis=
{
    .working=0,
    .workMode=0,
    .zeta=0.f,
    .lf=&DJI_Motor[LF],
    .lb=&DJI_Motor[LB],
    .rf=&DJI_Motor[RF],
    .rb=&DJI_Motor[RB],
    .yaw=&DJI_Motor[YAW],
};
uint32_t btl_test;
void MoveTask(void const * argument)
{
//    TickType_t systick = 0;
	static uint8_t lastMode;
		
	//              死区 积分限幅 总限幅  p    i  d 
	float move_pid[6]={0,   1000, 16300,9.f,0.1f,0};
    for(int i=0;i<4;i++)    
    {		
		PID_Init(&DJI_Motor[i].pid,move_pid);
		//原//PID_Init(&DJI_Motor[i].pid,9.f,0.1f,0.f,1200,2000,15200,1000,0,16300);
    }
	
    
    while (1)
    {
		btl_test++;
//        systick = osKernelSysTick();
        /*固有信息获取*/
		chassis.working = mcu_CAN.gimbal.dr16.chassisWork;
        chassis.workMode = mcu_CAN.gimbal.dr16.chassisMode;

		chassis.zeta = 45.5f - chassis.yaw->Data.angle;
		if(chassis.zeta > 180.f)chassis.zeta -= 360.f;
		if(chassis.zeta <-180.f)chassis.zeta += 360.f;

		if(lastMode == 1)
		{
			if(ABS(chassis.zeta)>5.f)
			{
				chassis.workMode = 1;
			}
		}
		lastMode = chassis.workMode;

        /* code */
        if(chassis.working)
        {
        /*云台系转至底盘*/
            chassis.vx = mcu_CAN.gimbal.dr16.vy*arm_sin_f32(chassis.zeta*0.0176533f)*26.f
                       + mcu_CAN.gimbal.dr16.vx*arm_cos_f32(chassis.zeta*0.0176533f)*26.f;
            chassis.vy = mcu_CAN.gimbal.dr16.vy*arm_cos_f32(chassis.zeta*0.0176533f)*26.f
                       - mcu_CAN.gimbal.dr16.vx*arm_sin_f32(chassis.zeta*0.0176533f)*26.f;

            chassis.vw = chassis_vw_get(chassis.workMode);

        /*麦轮底盘控制 入参为速度三元数*/
            mecanum_chassis_ctrl(chassis.vx,chassis.vy,chassis.vw);
        }
        else
        {
            /*卸力*/
			for(int i=0;i<4;i++)
			{
				DJI_Motor[i].pid.param.MaxOut =0;
			}
        }
        vTaskDelay(1);
    }
}

float chassis_vw_get(uint8_t mode)
{
    float vw;
    switch(mode)
    {
        /*跟随*/
        case 0:
            vw = 180.f*chassis.zeta
               - 23.f*(chassis.yaw->Data.velocity);
            VAL_Limit(vw,-3200,3200);
        break;

        /*小陀螺*/
        case 1:
            vw = 3700.f+game_robot_state.robot_level*900.f;
            VAL_Limit(vw,-VW_MAX,VW_MAX);
        break;

        /*右45°*/
        case 2:
            vw = 180.f*(chassis.zeta - 45.0f)
               - 23.f*(chassis.yaw->Data.velocity);
            VAL_Limit(vw,-2800,2800);
        break;

        /*左45度*/
        case 3:
            vw = 180.f*(chassis.zeta + 45.0f)
               - 23.f*(chassis.yaw->Data.velocity);
            VAL_Limit(vw,-2800,2800);
        break;
        
		/*突破*/
		case 4:
			vw = 60.f*chassis.zeta
               - 3.f*(chassis.yaw->Data.velocity);
            VAL_Limit(vw,-800,800);
        break;
		/*分离*/
		case 5:
			vw = 0;
            VAL_Limit(vw,0,0);
		break;
    }
    return vw;
}

void mecanum_chassis_ctrl(float vx,float vy,float vw)
{
    float ExpectV[4];
    //麦轮全向解算（规定左侧轮子向前转为正，完成电流极性处理）

    ExpectV[LF] = ( vy + vx + vw);
    ExpectV[LB] = ( vy - vx + vw);
    ExpectV[RF] = (-vy + vx + vw);
    ExpectV[RB] = (-vy - vx + vw);

    for(int i=0;i<4;i++)
    {
        VAL_Limit(ExpectV[i],-16300,16300);
        f_PID_Calculate(&DJI_Motor[i].pid, ExpectV[i] , DJI_Motor[i].Data.velocity);
    }

	//功率控制(完成电机控制)
#if POWER_CTRL_ENABLE
	chassis_power_limit();
#endif
	
}
double scaling[4];
double chassis_pidout = 0.f;
float  klimit = 0.f,plimit = 0.f;
float  chassis_pidout_max = 65536.f;
void chassis_power_limit(void)
{
	if(power_heat_data.chassis_power>960)
	{
		for(int i=0;i<4;i++)
		{
			VAL_Limit(DJI_Motor[i].pid.param.MaxOut,-4096,4096);
		}
	}
	else
	{
		chassis_pidout = ABS(DJI_Motor[0].pid.Err)
					   + ABS(DJI_Motor[1].pid.Err)
					   + ABS(DJI_Motor[2].pid.Err)
					   + ABS(DJI_Motor[3].pid.Err);
		for(int i=0;i<4;i++)
		{
			scaling[i] = DJI_Motor[i].pid.Err/chassis_pidout;
		}
		klimit = chassis_pidout/4096.0f;
		VAL_Limit(klimit,-1,1);
		
		if(power_heat_data.chassis_power_buffer<50
				&&power_heat_data.chassis_power_buffer>=40)	plimit=0.9;
		else if(power_heat_data.chassis_power_buffer<40
				&&power_heat_data.chassis_power_buffer>=35)	plimit=0.75;
		else if(power_heat_data.chassis_power_buffer<35
				&&power_heat_data.chassis_power_buffer>=30)	plimit=0.5;
		else if(power_heat_data.chassis_power_buffer<30
				&&power_heat_data.chassis_power_buffer>=20)	plimit=0.25;
		else if(power_heat_data.chassis_power_buffer<20
				&&power_heat_data.chassis_power_buffer>=10)	plimit=0.125;
		else if(power_heat_data.chassis_power_buffer<10)	plimit=0.05;
		
		else if(power_heat_data.chassis_power_buffer==60)	plimit=1;
		
		for(int i=0;i<4;i++)
		{
			DJI_Motor[i].pid.param.MaxOut = chassis_pidout_max * scaling[i] * klimit * plimit;
		}
	}
}

