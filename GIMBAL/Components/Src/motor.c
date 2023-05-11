//
// Created by YanYuanbin on 22-3-30.
//
#include "motor.h"
#include "pid.h"


//云台电机相关_待修改

DJI_MOTOR DJI_Motor[MOTOR_NUM]={
    [YAW]=
    {
        .type=DJI_GM6020,
        .usage=YAW,
        .CANx=CAN2,
        .Data.StdId=0x205,
        .txMsg=&CAN_TxMsg[_CAN2][_0x1ff],
    },
    [PITCH]=
    {
        .type=DJI_GM6020,
        .usage=PITCH,
        .CANx=CAN1,
        .Data.StdId=0x206,
        .txMsg=&CAN_TxMsg[_CAN1][_0x1ff],
    },
    [SHOOT_L]=
    {
        .type=DJI_M3508S,
        .usage=SHOOT_L,
        .CANx=CAN1,
        .Data.StdId=0x207,
        .txMsg=&CAN_TxMsg[_CAN1][_0x1ff],
    },
    [SHOOT_R]=
    {
        .type=DJI_M3508S,
        .usage=SHOOT_R,
        .CANx=CAN1,
        .Data.StdId=0x205,
        .txMsg=&CAN_TxMsg[_CAN1][_0x1ff],
    },
    [TRIGGER]=
    {
        .type=DJI_M2006,
        .usage=TRIGGER,
        .CANx=CAN1,
        .Data.StdId=0x208,
        .txMsg=&CAN_TxMsg[_CAN1][_0x1ff],
    }
};


/**
  * @brief  transform the encoder(0-8192) to anglesum(3.4E38)
  * @param  *Info        pointer to a General_Motor_Info_t structure that 
	*					             contains the infomation for the specified motor
  * @param  torque_ratio the specified motor torque ratio
  * @param  MAXencoder   the specified motor max encoder number
  * @retval anglesum
  */
static float encoder_to_anglesum(General_Motor_Info_t *Info,float torque_ratio,uint16_t MAXencoder)
{
		int16_t res1 = 0,res2 =0;
		
		if(Info == NULL) return 0;
		
		/* check the motor init */
		if(Info->init != true)
		{
			/* config the init flag */
			Info->init = true;
			/* update the last encoder */
			Info->last_encoder = Info->encoder;
			/* reset the angle */
			Info->angle = 0;
		}
		
		/* get the possiable min encoder err */
		if(Info->encoder < Info->last_encoder)
		{
        res1 = Info->encoder-Info->last_encoder + MAXencoder;
		}
		else if(Info->encoder > Info->last_encoder)
		{
        res1 = Info->encoder-Info->last_encoder - MAXencoder;
		}
		res2 = Info->encoder-Info->last_encoder;
		
		/* update the last encoder */
		Info->last_encoder = Info->encoder;
		
		/* transforms the encoder data to tolangle */
		ABS(res1) > ABS(res2) ? (Info->angle += (float)res2/(MAXencoder*torque_ratio)*360.f) : (Info->angle += (float)res1/(MAXencoder*torque_ratio)*360.f);
		
		return Info->angle;
}

/**
  * @brief  float loop constrain
  * @param  Input    the specified variables
  * @param  minValue minimum number of the specified variables
  * @param  maxValue maximum number of the specified variables
  * @retval variables
  */
static float f_loop_constrain(float Input, float minValue, float maxValue)
{
    if (maxValue < minValue)
    {
        return Input;
    }

    if (Input > maxValue)
    {
        float len = maxValue - minValue;
        while (Input > maxValue)
        {
            Input -= len;
        }
    }
    else if (Input < minValue)
    {
        float len = maxValue - minValue;
        while (Input < minValue)
        {
            Input += len;
        }
    }
    return Input;
}

/**
  * @brief  transform the encoder(0-8192) to angle(-180-180)
  * @param  *Info        pointer to a General_Motor_Info_t structure that 
	*					             contains the infomation for the specified motor
  * @param  torque_ratio the specified motor torque ratio
  * @param  MAXencoder   the specified motor max encoder number
  * @retval angle
  */
static float encoder_to_angle(General_Motor_Info_t *Info,float torque_ratio,uint16_t MAXencoder)
{
		float encoder_err = 0.f;
	
		if(Info == NULL) return 0.f;
		
		/* check the motor init */
		if(Info->init != true)
		{
			/* config the init flag */
			Info->init = true;
			/* update the last encoder */
			Info->last_encoder = Info->encoder;
			/* reset the angle */
			Info->angle = 0;
		}
		
		encoder_err = Info->encoder - Info->last_encoder;
		
		/* 0↓ -> MAXencoder */		
		if(encoder_err > MAXencoder*0.5f)
		{
			Info->angle += (float)(encoder_err - MAXencoder)/(MAXencoder*torque_ratio)*360.f;
		}
		/* MAXencoder↑ -> 0 */		
		else if(encoder_err < -MAXencoder*0.5f)
		{
			Info->angle += (float)(encoder_err + MAXencoder)/(MAXencoder*torque_ratio)*360.f;
		}
		/* 未过0点 */
		else
		{
			Info->angle += (float)(encoder_err)/(MAXencoder*torque_ratio)*360.f;
		}
		
		/* update the last encoder */
		Info->last_encoder = Info->encoder;
		
		/* loop constrain */
		f_loop_constrain(Info->angle,-180,180);

		return Info->angle;
}


/**
  * @brief  transform the DJI motor receive data
  * @param  StdId  specifies the standard identifier.
  * @param  *rxBuf can receive memory address
  * @param  *DJI_Motor pointer to a DJI_Motor_Info_t structure that contains the information of DJI motor
  * @retval None
  */
void get_DJI_Motor_Info(uint32_t StdId, uint8_t *rxBuf,DJI_MOTOR *DJI_Motor)
{
	/* check the StdId */
	if(StdId != DJI_Motor->Data.StdId) return;
	
	/* transforms the  general motor data */
	DJI_Motor->I_Level = LOW;
	DJI_Motor->Data.temperature = rxBuf[6];
	DJI_Motor->Data.encoder  = ((int16_t)rxBuf[0]<<8 | (int16_t)rxBuf[1]);
	DJI_Motor->Data.velocity = ((int16_t)rxBuf[2]<<8 | (int16_t)rxBuf[3]);
	DJI_Motor->Data.current  = ((int16_t)rxBuf[4]<<8 | (int16_t)rxBuf[5]);
	
	/* transform the encoder to anglesum */
	switch(DJI_Motor->type)
	{
		case DJI_GM6020:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,1.f,8192);
		break;
	
		case DJI_M3508:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,3591.f/187.f,8192);
		break;
		
		case DJI_M3508S:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,1,8192);
		break;
		
		case DJI_M2006:
			DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data,36.f,8192);
		break;
		
		default:break;
	}	
	//发送部分
        //数据装载
        DJI_Motor->txMsg->data[CAN_L[DJI_Motor->Data.StdId - (DJI_Motor->type==DJI_GM6020?0x205:0x201)]]
            = (uint8_t)((int16_t)(DJI_Motor->pid.Output)>>8);
        DJI_Motor->txMsg->data[CAN_H[DJI_Motor->Data.StdId - (DJI_Motor->type==DJI_GM6020?0x205:0x201)]]
            = (uint8_t)((int16_t)(DJI_Motor->pid.Output));
				
//		  //堵转检测
//		static uint32_t _time[MOTOR_NUM];
//		if(DJI_Motor->I_Level != HIGH){
//			_time[DJI_Motor->usage] = osKernelSysTick();
//		}
//		if(osKernelSysTick() - _time[DJI_Motor->usage] > 500||DJI_Motor->stalled==1){ //发生堵转
//			DJI_Motor->stalled = 1;
//			DJI_Motor->txMsg->data[CAN_L[DJI_Motor->Data.StdId - (DJI_Motor->type==DJI_GM6020?0x205:0x201)]] = 0;
//			DJI_Motor->txMsg->data[CAN_H[DJI_Motor->Data.StdId - (DJI_Motor->type==DJI_GM6020?0x205:0x201)]] = 0;
//		}
	
}


/**
  * @brief  transform the RMD motor receive data
  * @param  StdId  specifies the standard identifier.
  * @param  *rxBuf can receive memory address
  * @param  *RMD_Motor pointer to a RMD_L9025_Info_t structure that contains the information of RMD motor
  * @retval None
  */
void get_RMD_Motor_Info(uint32_t StdId, uint8_t *rxBuf,RMD_L9025_Info_t *RMD_Motor)
{
	/* check the StdId */
	if(StdId != RMD_Motor->Data.StdId) return;
	
	RMD_Motor->order = rxBuf[0];
	
	/* transforms the  general motor data */
	RMD_Motor->Data.temperature = rxBuf[1];
	RMD_Motor->Data.current  = ((int16_t)(rxBuf[2]) | (int16_t)(rxBuf[3]<<8));
	RMD_Motor->Data.velocity = ((int16_t)(rxBuf[4]) | (int16_t)(rxBuf[5]<<8));
	RMD_Motor->Data.encoder  = ((int16_t)(rxBuf[6]) | (int16_t)(rxBuf[7]<<8));
	
	RMD_Motor->Data.angle = encoder_to_anglesum(&RMD_Motor->Data,1.f,32768);
}
