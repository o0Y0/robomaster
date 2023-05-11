//
// Created by YanYuanbin on 22-10-3.
//

#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "can.h"

typedef struct {
	CAN_TypeDef *Instance;
    CAN_RxHeaderTypeDef header;
    uint8_t 			data[8];
} CAN_RxFrameTypeDef;

typedef struct {
	CAN_TypeDef *Instance;
    CAN_TxHeaderTypeDef header;
    uint8_t				data[8];
}CAN_TxFrameTypeDef;

/* Exported functions --------------------------------------------------------*/
extern void can_init(void);

enum{
    _CAN1,
    _CAN2,
    CAN_PORT_NUM,
};

enum{
    _0x200,//3508 2006 ID
    _0x1ff,//6020 ID
    _0x300,//上板发送ID
    _0x301,//下板发送ID
    stdID_NUM,
};

typedef struct
{
    struct{
        /*通信ID*/
        uint32_t ID;
        CAN_TxFrameTypeDef *txMsg;

        struct{
            /*各类状态*/
            uint8_t allState;
			/*超电开关*/
			uint8_t supercapWork;
            /*底盘开关*/
            uint8_t chassisWork;
            /*底盘工作模式  0跟随 1小陀螺 2右45度 3左45度 4飞坡 5分离*/
            uint8_t chassisMode;
            /*速度元素 */
            int16_t vx,vy;
        }dr16;
    }gimbal;

    struct{
        /*通信ID*/
        uint32_t ID;
        CAN_TxFrameTypeDef *txMsg;

        struct{
            int16_t shoot_remainingHeat;
            int16_t shoot_speedGain;
            uint16_t shoot_expectSpeed;
        }referee;
    }chassis;

}MCU_COMMUNICATE;

extern MCU_COMMUNICATE mcu_CAN;

extern int CAN_L[8];
extern int CAN_H[8];

extern CAN_TxFrameTypeDef CAN_TxMsg[CAN_PORT_NUM][stdID_NUM];

#endif //BSP_CAN_H
