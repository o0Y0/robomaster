//
// Created by YanYuanbin on 22-10-3.
//
#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "can.h"

/*��CAN2���ϵ������*/
#define SEND2DRIVE_ENABLE 1
/*����̨�巢��*/
#define SEND2GIMBAL_ENABLE 1
/*��������*/
#define USE_SUPERCAP 0

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


enum{
    _CAN1,
    _CAN2,
    CAN_PORT_NUM,
};

typedef enum
{
    LF,
    LB,
    RF,
    RB,
    YAW,
    MOTOR_NUM,
}MOTOR_USAGE;


enum{
    _0x200,//3508 2006 ID
    _0x1ff,//6020 ID
    _0x300,//�ϰ巢��ID
    _0x301,//�°巢��ID
	_0x026,//�������ݷ���
    stdID_NUM,
};


typedef struct
{
    struct{
        /*ͨ��ID*/
        uint32_t ID;
        CAN_TxFrameTypeDef *txMsg;

        struct{
            /*����״̬*/
            uint8_t allState;
			/*���翪��*/
			uint8_t supercapWork;
            /*���̿��� 0�رգ�1������ݰ壬3������ݰ�*/
            uint8_t chassisWork;
            /*����ģʽ������̨���� 0ж�� 1���� 2���� 3С����*/
            uint8_t chassisMode;
            /*�ٶ�Ԫ�� */
            int16_t vx,vy;
        }dr16;
    }gimbal;

    struct{
        /*ͨ��ID*/
        uint32_t ID;
        CAN_TxFrameTypeDef *txMsg;

        struct{
            int16_t shoot_remainingHeat;
            int16_t shoot_speedGain;
            uint16_t shoot_expectSpeed;
        }referee;
    }chassis;
	
	struct{
		uint32_t ID;
        CAN_TxFrameTypeDef *txMsg;
		
		uint8_t mode;
		struct{
			int16_t cap_low_waring;
			int16_t DischargeATK;
			int16_t chassis_power;
			int16_t CapQuantity;
		}msg;	
		struct{
			uint8_t  SuperPower_cmd;
			
		}console;
	}supercap;
	
}MCU_COMMUNICATE;
extern MCU_COMMUNICATE mcu_CAN;

extern int CAN_L[8];
extern int CAN_H[8];

extern uint32_t pTxMailbox1;
extern uint32_t pTxMailbox2;

extern CAN_TxFrameTypeDef CAN_TxMsg[CAN_PORT_NUM][stdID_NUM];

//extern CAN_TxFrameTypeDef 

/* Exported functions --------------------------------------------------------*/
extern void can_init(void);

#endif //BSP_CAN_H
