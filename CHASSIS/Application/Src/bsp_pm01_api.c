#include "bsp_pm01_api.h"
#include "referee_info.h"
#include "bsp_can.h"

//#define Super_Power_Message_ID 0x026


static int32_t SuperCapSendMsg(void *argc)
{
    uint8_t cmd = mcu_CAN.supercap.console.SuperPower_cmd; //此为 键位开启判断
    uint8_t mode = 0;//模式默认（暂时）
    uint16_t ref_power_limit = (uint16_t)game_robot_state.chassis_power_limit;  //底盘功率限制
    uint16_t ref_power_buffer = (uint16_t)power_heat_data.chassis_power_buffer; //底盘功率缓存
    uint16_t powerbuffer_limit = (uint16_t)power_heat_data.chassis_power_buffer; 
    if(powerbuffer_limit > 60)
        powerbuffer_limit = 250.0f;
    else
        powerbuffer_limit = 60.0f;
	
	
	mcu_CAN.supercap.txMsg->data[0] = (uint8_t)cmd;
	mcu_CAN.supercap.txMsg->data[1] = (uint8_t)mcu_CAN.supercap.mode;
	mcu_CAN.supercap.txMsg->data[2] = (uint8_t)(ref_power_buffer >> 8);
	mcu_CAN.supercap.txMsg->data[3] = (uint8_t) ref_power_buffer;
	mcu_CAN.supercap.txMsg->data[4] = (uint8_t)(ref_power_limit >> 8);
	mcu_CAN.supercap.txMsg->data[5] = (uint8_t) ref_power_limit ;
	mcu_CAN.supercap.txMsg->data[6] = (uint8_t)(powerbuffer_limit >> 8);
	mcu_CAN.supercap.txMsg->data[7] = (uint8_t) powerbuffer_limit;
	
	HAL_CAN_AddTxMessage(&hcan2,&CAN_TxMsg[_CAN2][_0x026].header,CAN_TxMsg[_CAN2][_0x026].data,&pTxMailbox2);
    return 0;
}

