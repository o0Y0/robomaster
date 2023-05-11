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


/* 电机类型枚举 */
typedef enum{
    DJI_GM6020,
    DJI_M3508,
    DJI_M2006,
	 RMD_L9025,
	DJI_M3508S,//3508无减速箱
    MOTOR_TYPE_NUM,
}Motor_Type_e;

/* 通用电机信息 */
typedef struct
{
        bool init;
		uint32_t StdId;
        volatile int16_t current;  /*-32768~32768*/
        volatile int16_t velocity; /**/
        volatile int16_t encoder;  /*范围：0~8192*/
		volatile int16_t  last_encoder;
		volatile float angle;      /*范围：0~360*/
        volatile uint8_t temperature;
}General_Motor_Info_t;

/* DJI电机封装 */
typedef struct
{
	General_Motor_Info_t Data;
    Motor_Type_e Type;
}DJI_Motor_Info_t;

/* RMD_L_9025电机封装 */
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

    /*电机堵转标志，0正常，1堵转后卸力*/
    uint8_t stalled;

    /*载入PID，来自pid.c*/
    PID_TypeDef_t pid;

    /*CAN口配置，CAN1/CAN2*/
    CAN_TypeDef *CANx;

    /*载入CAN发送结构体，来自can.c中实例化的结构体数组CAN_TxMsg[_CANx][stdID]*/
    CAN_TxFrameTypeDef *txMsg;

    /*电机类型，3508，6020，2006，3508s为去掉减速箱的3508*/
    Motor_Type_e type;

    /*电机用途，描述电机的具体用途，比如摩擦轮，拨弹盘，底盘驱动，云台驱动等*/
    MOTOR_USAGE usage;

    /*电机电流水平，描述电机实际电流的大小，低，中，高*/
    MOTOR_CURRENT_LEVEL I_Level;
    
}DJI_MOTOR;

extern DJI_MOTOR DJI_Motor[MOTOR_NUM];


/* Exported functions --------------------------------------------------------*/
extern void get_DJI_Motor_Info(uint32_t StdId, uint8_t *rxBuf,DJI_MOTOR *DJI_Motor);
extern void get_RMD_Motor_Info(uint32_t StdId, uint8_t *rxBuf,RMD_L9025_Info_t *RMD_Motor);

#endif //MOTOR_H

