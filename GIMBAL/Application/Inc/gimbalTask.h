#ifndef __GIMBALTASK_H
#define __GIMBALTASK_H
#include "main.h"
#include "motor.h"

typedef struct 
{
    uint8_t working;

    uint8_t pitMode;/*pitchÖá¿ØÖÆÄ£Ê½£¬0ÍÓÂİÒÇ  1±àÂëÆ÷*/

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

