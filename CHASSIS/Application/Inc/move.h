#ifndef __MOVE_H
#define __MOVE_H
#include "main.h"
#include "motor.h"
#include "referee_info.h"

typedef struct
{
    /*工作状态  0关闭 1开启*/
    uint8_t working;
    
    /*工作模式  0跟随 1小陀螺 2右45度 3左45度 4飞坡*/
    uint8_t workMode;
    
    /*云台坐标与底盘坐夹角 【-180，180】*/
    float zeta;

    /*底盘速度三元数*/
    float vx,vy,vw;

    /*电容电压*/
    float cap_V;

    /*电机组成*/
    DJI_MOTOR *lf,*lb,*rf,*rb,*yaw;

}CHASSIS;


extern CHASSIS chassis;

void mecanum_chassis_ctrl(float vx,float vy,float vw);
float chassis_vw_get(uint8_t mode);
float spin_vw(void);
void chassis_power_limit(void);
#endif
