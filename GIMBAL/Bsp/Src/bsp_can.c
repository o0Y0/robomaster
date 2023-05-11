//
// Created by YanYuanbin on 22-10-3.
//
#include "bsp_can.h"
#include "message.h"
#include "motor.h"

/*向电机发送*/
#define SEND2MOTOR_ENABLE 1
/*向下板发送*/
#define SEND2CHASSIS_ENABLE 1

//电机CAN相关_待修改

int CAN_L[8] = { 0,2,4,6 , 0,2,4,6 };
int CAN_H[8] = { 1,3,5,7 , 1,3,5,7 };

uint32_t pTxMailbox1 =0;
uint32_t pTxMailbox2 =0;

MCU_COMMUNICATE mcu_CAN={
    .gimbal   ={.ID = 0x300,.txMsg = &CAN_TxMsg[_CAN2][_0x300]},
    .chassis  ={.ID = 0x301,.txMsg = &CAN_TxMsg[_CAN2][_0x301]},
};

CAN_TxFrameTypeDef CAN_TxMsg[CAN_PORT_NUM][stdID_NUM]= 
{
	[_CAN1]=
    {
        [_0x200]=
        {
            .header.StdId=0x200,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x1ff]=
        {
            .header.StdId=0x1ff,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x300]=
        {
            .header.StdId=0x300,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x301]=
        {
            .header.StdId=0x301,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

	},

	[_CAN2]=
    {
        [_0x200]=
        {
            .header.StdId=0x200,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x1ff]=
        {
            .header.StdId=0x1ff,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x300]=
        {
            .header.StdId=0x300,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },

        [_0x301]=
        {
            .header.StdId=0x301,
            .header.IDE=CAN_ID_STD,
            .header.RTR=CAN_RTR_DATA,
            .header.DLC=8,
        },
    },
};



/**
  * @brief  Configures the CAN Module.
  * @param  None
  * @retval None
  */
void can_init(void)
{
    CAN_FilterTypeDef sFilterConfig={0,};
		
		/* config the CAN_FilterInitStruct */
		sFilterConfig.FilterActivation = ENABLE;
		sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
		sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
		sFilterConfig.FilterIdHigh = 0x0000;
		sFilterConfig.FilterIdLow = 0x0000;
		sFilterConfig.FilterMaskIdHigh = 0x0000;
		sFilterConfig.FilterMaskIdLow = 0x0000;
		sFilterConfig.FilterBank = 0;
		sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
		sFilterConfig.SlaveStartFilterBank = 0;
		
		/* Configures the CAN1 reception filter according to the specified parameters in the CAN_FilterInitStruct */
		if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
		{	
		    Error_Handler();
		}
		
		/* Start the CAN1 module. */
		HAL_CAN_Start(&hcan1);
		
		/* Enable the CAN1 interrupts. */
		HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
		
		/* transform the CAN2 slave filter bank */
		sFilterConfig.SlaveStartFilterBank = 14;
		
		/* transform the CAN2 filter bank  */
		sFilterConfig.FilterBank = 14;
		
		/* Configures the CAN2 reception filter according to the specified parameters in the CAN_FilterInitStruct */
		if(HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
		{	
		    Error_Handler();
		}
		
		/* Start the CAN2 module. */
		HAL_CAN_Start(&hcan2);
		
		/* Enable the CAN2 interrupts. */
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
  * @brief  transform the CAN receive data.
  * @param  *can   hcan Register base address
  * @param  *canId specifies the standard identifier.
  * @param  *rxBuf can receive memory address
  * @retval None
  */
static void CAN_RxDataHandler(CAN_TypeDef *Instance,uint32_t *canId, uint8_t *rxBuf)
{	
	/* check the CAN allocation*/
	if(Instance == NULL)return;
	
	/* CAN1 RX interrupts */
	if(Instance == CAN1)
	{
		get_DJI_Motor_Info(*canId,rxBuf,&DJI_Motor[PITCH   ]);
		get_DJI_Motor_Info(*canId,rxBuf,&DJI_Motor[SHOOT_L ]);
		get_DJI_Motor_Info(*canId,rxBuf,&DJI_Motor[SHOOT_R ]);
		get_DJI_Motor_Info(*canId,rxBuf,&DJI_Motor[TRIGGER]);
		
#if SEND2MOTOR_ENABLE
		HAL_CAN_AddTxMessage(&hcan1,&CAN_TxMsg[_CAN1][_0x1ff].header,CAN_TxMsg[_CAN1][_0x1ff].data,&pTxMailbox1);
		HAL_CAN_AddTxMessage(&hcan1,&CAN_TxMsg[_CAN1][_0x200].header,CAN_TxMsg[_CAN1][_0x200].data,&pTxMailbox1);
#endif
	}
	/* CAN2 RX interrupts */
	else if(Instance == CAN2)
	{
		//yaw轴电机
		get_DJI_Motor_Info(*canId,rxBuf,&DJI_Motor[YAW]);
		
		//下板信息
		if(*canId == mcu_CAN.chassis.ID)
		{
			mcu_CAN.chassis.referee.shoot_remainingHeat  = (int16_t)(rxBuf[0]<<8);
			mcu_CAN.chassis.referee.shoot_remainingHeat  = (int16_t)(rxBuf[1]);
			mcu_CAN.chassis.referee.shoot_speedGain = (int16_t)(rxBuf[2]<<8);
			mcu_CAN.chassis.referee.shoot_speedGain = (int16_t)(rxBuf[3]);
			mcu_CAN.chassis.referee.shoot_expectSpeed = (uint16_t)(rxBuf[4]<<8);
			mcu_CAN.chassis.referee.shoot_expectSpeed = (uint16_t)(rxBuf[5]);
		}
		
#if SEND2CHASSIS_ENABLE
		mcu_CAN.gimbal.txMsg->data[1]= mcu_CAN.gimbal.dr16.chassisWork;
		mcu_CAN.gimbal.txMsg->data[1] = mcu_CAN.gimbal.dr16.chassisMode;
		mcu_CAN.gimbal.txMsg->data[2] = (uint8_t)(mcu_CAN.gimbal.dr16.vx>>8);
		mcu_CAN.gimbal.txMsg->data[3] = (uint8_t)(mcu_CAN.gimbal.dr16.vx);
		mcu_CAN.gimbal.txMsg->data[4] = (uint8_t)(mcu_CAN.gimbal.dr16.vy>>8);
		mcu_CAN.gimbal.txMsg->data[5] = (uint8_t)(mcu_CAN.gimbal.dr16.vy);
		mcu_CAN.gimbal.txMsg->data[6] = mcu_CAN.gimbal.dr16.supercapWork;
		mcu_CAN.gimbal.txMsg->data[7] = mcu_CAN.gimbal.dr16.allState;

		HAL_CAN_AddTxMessage(&hcan2,&CAN_TxMsg[_CAN2][_0x300].header,CAN_TxMsg[_CAN2][_0x300].data,&pTxMailbox2);
#endif

#if SEND2MOTOR_ENABLE
		HAL_CAN_AddTxMessage(&hcan2,&CAN_TxMsg[_CAN2][_0x200].header,CAN_TxMsg[_CAN2][_0x200].data,&pTxMailbox2);
		HAL_CAN_AddTxMessage(&hcan2,&CAN_TxMsg[_CAN2][_0x1ff].header,CAN_TxMsg[_CAN2][_0x1ff].data,&pTxMailbox2);
#endif
		
	}
}

/**
  * @brief  Rx FIFO 0 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{	
	static CAN_RxFrameTypeDef CAN_RxInstance ={0,};
	
	/* Get the hcan Register base address */
	CAN_RxInstance.Instance = hcan->Instance;
	
	/* Get an CAN frame from the Rx FIFO zone into the message RAM. */
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxInstance.header, CAN_RxInstance.data);
	
	/* transform the receive data */
	CAN_RxDataHandler(CAN_RxInstance.Instance,&CAN_RxInstance.header.StdId,CAN_RxInstance.data);
}
