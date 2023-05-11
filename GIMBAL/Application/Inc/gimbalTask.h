#ifndef __GIMBALTASK_H
#define __GIMBALTASK_H
#include "main.h"
#include "motor.h"

typedef struct 
{
    uint8_t working;

    uint8_t pitMode;/*pitch�����ģʽ��0������  1������*/

    float yawReal,pitReal[2];

    float yawTarget,pitTarget[2];

    DJI_MOTOR *yaw,*pit;
}GIMBAL;
extern GIMBAL gimbal;

void posture_ctrl(void);
void move_ctrl(void);
float ramp_fuction(float final, float now, float ramp);
//void send2vision(void);
#endif

