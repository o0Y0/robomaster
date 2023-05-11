#ifndef __MOVE_H
#define __MOVE_H
#include "main.h"
#include "motor.h"
#include "referee_info.h"

typedef struct
{
    /*����״̬  0�ر� 1����*/
    uint8_t working;
    
    /*����ģʽ  0���� 1С���� 2��45�� 3��45�� 4����*/
    uint8_t workMode;
    
    /*��̨������������н� ��-180��180��*/
    float zeta;

    /*�����ٶ���Ԫ��*/
    float vx,vy,vw;

    /*���ݵ�ѹ*/
    float cap_V;

    /*������*/
    DJI_MOTOR *lf,*lb,*rf,*rb,*yaw;

}CHASSIS;


extern CHASSIS chassis;

void mecanum_chassis_ctrl(float vx,float vy,float vw);
float chassis_vw_get(uint8_t mode);
float spin_vw(void);
void chassis_power_limit(void);
#endif
