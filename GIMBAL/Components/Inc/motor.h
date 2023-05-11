//
// Created by YanYuanbin on 22-10-3.
//

#ifndef MOTOR_H
#define MOTOR_H

#include "stdint.h"
#include "stdbool.h"
#include "robot_ref.h"
#include "pid.h"
#include "bsp_can.h"


/* �������ö�� */
typedef enum{
    DJI_GM6020,
    DJI_M3508,
    DJI_M2006,
	 RMD_L9025,
	DJI_M3508S,//3508�޼�����
    MOTOR_TYPE_NUM,
}Motor_Type_e;

/* ͨ�õ����Ϣ */
typedef struct
{
        bool init;
		uint32_t StdId;
        volatile int16_t current;  /*-32768~32768*/
        volatile int16_t velocity; /**/
        volatile int16_t encoder;  /*��Χ��0~8192*/
		volatile int16_t  last_encoder;
		volatile float angle;      /*��Χ��0~360*/
        volatile uint8_t temperature;
}General_Motor_Info_t;

/* DJI�����װ */
typedef struct
{
	General_Motor_Info_t Data;
    Motor_Type_e Type;
}DJI_Motor_Info_t;

/* RMD_L_9025�����װ */
typedef struct
{
	int8_t order;
	General_Motor_Info_t Data;
    Motor_Type_e Type;
}RMD_L9025_Info_t;


typedef enum
{
    YAW,
    PITCH,
    SHOOT_L,
    SHOOT_R,
	TRIGGER,

    MOTOR_NUM,
}MOTOR_USAGE;

typedef enum
{
    LOW,
    MID,
    HIGH,

    CURRENT_LEVEL_NUM,
}MOTOR_CURRENT_LEVEL;

typedef struct
{
    General_Motor_Info_t Data;

    /*�����ת��־��0������1��ת��ж��*/
    uint8_t stalled;

    /*����PID������pid.c*/
    PID_TypeDef_t pid;

    /*CAN�����ã�CAN1/CAN2*/
    CAN_TypeDef *CANx;

    /*����CAN���ͽṹ�壬����can.c��ʵ�����Ľṹ������CAN_TxMsg[_CANx][stdID]*/
    CAN_TxFrameTypeDef *txMsg;

    /*������ͣ�3508��6020��2006��3508sΪȥ���������3508*/
    Motor_Type_e type;

    /*�����;����������ľ�����;������Ħ���֣������̣�������������̨������*/
    MOTOR_USAGE usage;

    /*�������ˮƽ���������ʵ�ʵ����Ĵ�С���ͣ��У���*/
    MOTOR_CURRENT_LEVEL I_Level;
    
}DJI_MOTOR;

extern DJI_MOTOR DJI_Motor[MOTOR_NUM];


/* Exported functions --------------------------------------------------------*/
extern void get_DJI_Motor_Info(uint32_t StdId, uint8_t *rxBuf,DJI_MOTOR *DJI_Motor);
extern void get_RMD_Motor_Info(uint32_t StdId, uint8_t *rxBuf,RMD_L9025_Info_t *RMD_Motor);

#endif //MOTOR_H

