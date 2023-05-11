#include "gimbalTask.h"
#include "cmsis_os.h"
#include "bsp_rc.h"
#include "bsp_can.h"
#include "INS_Task.h"
#include "vision.h"
#include "pid.h"

extern float INS_angle[3];


GIMBAL gimbal=
{
    .working=0,
    .pitMode=0,
    .pit=&DJI_Motor[PITCH],
    .yaw=&DJI_Motor[YAW],
};
void gimbalTask(void const * argument)
{
			
	//              死区 积分限幅 总限幅 p      i     d      //pid移植有问题、关于I积分项 
	float yaw_pid[6]={0,   2000, 0.f,65.f,0.01f, 0 },// i 1.5f    30000
		  pit_pid[6]={0,   9000, 0.f,72.f, 0.f , 0 };//   0.6f 总限幅30000
	//待修改云台PID
    PID_Init(&gimbal.yaw->pid,yaw_pid);
    PID_Init(&gimbal.pit->pid,pit_pid);
    
//    TickType_t 0 = 0;

    while (1)
    {
//        systick = osKernelSysTick();
        gimbal.yawReal = INS_Info.yaw_angle;
        gimbal.pitReal[0] = INS_Info.pit_angle;
        gimbal.pitReal[1] = gimbal.pit->Data.angle;
		
        /*云台姿态控制*/
        posture_ctrl();
		/*视觉信息更新（发送部分）*/
//		send2vision();
		
        if(gimbal.working)
        {
            gimbal.pitTarget[!gimbal.pitMode] = gimbal.pitReal[!gimbal.pitMode];

            /*云台期望姿态*/
            gimbal.yawTarget -= rc_ctrl.rc.ch[0]*0.0003f
							  + rc_ctrl.mouse.x*0.0012f;

            gimbal.pitTarget[gimbal.pitMode] += rc_ctrl.rc.ch[1]*0.0001f
											  - rc_ctrl.mouse.y*0.0036f;


            /*云台移动期望速度（底盘带动云台在平面运动，所以是传给底盘的）*/
            mcu_CAN.gimbal.dr16.vx = ramp_fuction(  rc_ctrl.rc.ch[2]*2.3333f
                                                    + 5500*(rc_ctrl.key.set.D-rc_ctrl.key.set.A)//对应遥控器最大值
                                                    ,mcu_CAN.gimbal.dr16.vx,
                                                    5500/900.f
                                                );

            mcu_CAN.gimbal.dr16.vy = ramp_fuction(  rc_ctrl.rc.ch[3]*2.3333f
                                                    + 5500*(rc_ctrl.key.set.W-rc_ctrl.key.set.S)//对应遥控器最大值
                                                    ,mcu_CAN.gimbal.dr16.vy,
                                                    5500/1100.f
                                                );
        }
        else
        {
            gimbal.yawTarget = INS_Info.yaw_angle;
            gimbal.pitTarget[0] = INS_Info.pit_angle;
            gimbal.pitTarget[1] = gimbal.pit->Data.angle;
            mcu_CAN.gimbal.dr16.vx = 0;
            mcu_CAN.gimbal.dr16.vy = 0;
        }

        vTaskDelay(1);
//		osDelay(1);
    }
}

float ramp_fuction(float final, float now, float ramp)
{
    float buffer = 0;
	buffer = final - now;
	if(final!=0)
	{
		if (buffer > 0){
			if (buffer > ramp){  
				now += ramp;
			}else{
				now += buffer;
			}
		}else{
			if (buffer < -ramp){
				now -= ramp;
			}else{
				now += buffer;
			}
		}
	}
	else
	{
		if (buffer > 0){
			if (buffer > ramp){  
				now += ramp*5.f;
			}
		}else{
			if (buffer < -ramp){
				now -= ramp*5.f;
			}
		}
	}
	return now;
}
uint8_t visiontast=0;
void posture_ctrl(void)
{
    float angle_Err[2],speed_Err[2];

//期望角度处理
    //YAW轴
    if(gimbal.yawTarget>180.0f)
    {
        gimbal.yawTarget-=360.0f;
    }
    if(gimbal.yawTarget<-180.0f)
    {
        gimbal.yawTarget+=360.0f;
    }
    //PITCH轴
    VAL_Limit(gimbal.pitTarget[0],-10.5f,26.5f);//陀螺仪模式
    VAL_Limit(gimbal.pitTarget[1],235.f,267.f);//编码器模式

//角度误差计算
    //常规控制
    angle_Err[0] = gimbal.yawTarget - gimbal.yawReal;
    angle_Err[1] = gimbal.pitReal[gimbal.pitMode] - gimbal.pitTarget[gimbal.pitMode];//陀螺仪模式
//角度误差处理
    if(angle_Err[0]>=180.0f)angle_Err[0]-=360.0f;
    if(angle_Err[0]<-180.0f)angle_Err[0]+=360.0f;
    //自瞄
    if((rc_ctrl.mouse.press_r ||visiontast)){
        /*关闭激光*/
        HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_8);
        if(tx2.isFind==1){
            //角度误差   =         视觉计算        +          前馈补偿（now-last)*k
            angle_Err[0] =  tx2.yaw_Err[0];
            angle_Err[1] = -tx2.pit_Err[0];
            gimbal.yawTarget = INS_Info.yaw_angle;
            gimbal.pitTarget[0] = INS_Info.pit_angle;
            gimbal.pitTarget[1] = gimbal.pit->Data.angle;
        }
    }else {
        /*打开激光*/
        HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_8);
    }

//角度误差限幅
    VAL_Limit(angle_Err[0],-30.0f,30.0f);
    VAL_Limit(angle_Err[1],-30.0f,30.0f);
//速度误差计算
    speed_Err[0] = angle_Err[0]*55.f - INS_Info.yaw_gyro; //57.29578f
    speed_Err[1] = INS_Info.pit_gyro - angle_Err[1]*57.29578f;
//速度控制(PID控制方案)
    f_PID_Calculate(&gimbal.yaw->pid,speed_Err[0],0);
    f_PID_Calculate(&gimbal.pit->pid,speed_Err[1],0);
}

float lastAngle;
float curAngle;
double angleSum;
float dlfjlkdf(float INS_angle)
{

	curAngle = INS_angle - lastAngle;
	lastAngle = INS_angle;
	
	if(curAngle<=-3.1415926f)
	{
		curAngle+=3.1415926f*2.f;
	}
	if(curAngle>=3.1415926f)
	{
		curAngle-=3.1415926f*2.f;
	}
	angleSum+=curAngle;
	
	return angleSum;
}

////视觉发送
//void send2vision(void)
//{
//	float yawAngle = dlfjlkdf(INS_angle[2]);
//    UART1_TX_BUF[0]  = 0x69;
//    UART1_TX_BUF[1]  = 0;//敌方颜色：0蓝，2红
//    UART1_TX_BUF[2]  = 1;
//    UART1_TX_BUF[3]  = tx2.kf_Flag;
//    UART1_TX_BUF[4]  = tx2.enemyMode;
//    UART1_TX_BUF[5]  = 0;
//    if(tx2.enemyMode == 0){
//        UART1_TX_BUF[6]  = (int16_t)(yawAngle);
//        UART1_TX_BUF[7]  = (int16_t)((yawAngle)*100.f)%100;
//        UART1_TX_BUF[8]  = (int16_t)((yawAngle)*10000.f)%100;
//        UART1_TX_BUF[9]  = (int16_t)(-INS_angle[0]);
//        UART1_TX_BUF[10] = (int16_t)((-INS_angle[0])*100.f)%100;
//        UART1_TX_BUF[11] = (int16_t)((-INS_angle[0])*10000.f)%100;
//    }
//    else{
//        UART1_TX_BUF[6]  = (int16_t)(gimbal.yaw->Data.angle);
//        UART1_TX_BUF[7]  = (int16_t)((gimbal.yaw->Data.angle)*100.f)%100;
//        UART1_TX_BUF[8]  = (int16_t)((gimbal.yaw->Data.angle)*10000.f)%100;
//        UART1_TX_BUF[9]  = (int16_t)(gimbal.pit->Data.angle);
//        UART1_TX_BUF[10] = (int16_t)((gimbal.pit->Data.angle)*100.f)%100;
//        UART1_TX_BUF[11] = (int16_t)((gimbal.pit->Data.angle)*10000.f)%100;
//    }
//	UART1_TX_BUF[12] = (UART1_TX_BUF[1]+UART1_TX_BUF[2]+UART1_TX_BUF[3] + UART1_TX_BUF[4]+UART1_TX_BUF[5] + UART1_TX_BUF[6]
//					 + UART1_TX_BUF[7]+UART1_TX_BUF[8] + UART1_TX_BUF[9]+UART1_TX_BUF[10]+UART1_TX_BUF[11])/11;
//	
//	UART1_TX_BUF[13] = 0x96;
//		
//    DMA_Cmd(DMA2_Stream6,ENABLE);
//}

