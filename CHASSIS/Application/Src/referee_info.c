#include "referee_info.h"


//修改_裁判系统
ext_game_status_t                   game_status;                  //0x0001
ext_event_data_t                    event_data;                   //0x0101
ext_game_robot_status_t             game_robot_state;             //0x0201
ext_power_heat_data_t 	        	power_heat_data;	          //0x0202
ext_buff_t                          buff_data;                    //0x0204
ext_robot_hurt_t                    armor_state_data;             //0x0206
ext_shoot_data_t                    real_shoot_data;              //0x0207
ext_bullet_remaining_t              bullet_remaining;             //0x0208\


/*
*@title：float8位转32位
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
float bit8TObit32(uint8_t *change_info)
{
	union
	{
    float f;
		char  byte[4];
	}u32val;
    u32val.byte[0]=change_info[0];
    u32val.byte[1]=change_info[1];
    u32val.byte[2]=change_info[2];
    u32val.byte[3]=change_info[3];
	return u32val.f;
} 
/*
*@title：int32位转8位
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
 char bit32TObit8(int index_need,int bit32)
{
	union
	{
    float  f;
		char  byte[4];
	}u32val;
    u32val.f = bit32;
	return u32val.byte[index_need];
}
/*
*@title：int16_t8位转16位
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
int16_t bit8TObit16(uint8_t *change_info)
{
	union
	{
    int16_t f;
		char  byte[2];
	}u16val;
    u16val.byte[0]=change_info[0];
    u16val.byte[1]=change_info[1];
	return u16val.f;
}
/*
*@title：int16位转8位
*@description：
*@param 1：	
*@param 2：	
*@return:：	
*/
 char bit16TObit8(int index_need,int bit16)
{
	union
	{
    float  f;
		char  byte[2];
	}u16val;
    u16val.f = bit16;
	return u16val.byte[index_need];
}
/*
   0：未开始比赛；
   1：准备阶段；
   2：自检阶段；
   3：5s 倒计时；
   4：对战中；
   5：比赛结算中
*/
uint16_t length;
void RefereeInfo_Decode(uint8_t *UsartRx_Info)
{

	switch(bit8TObit16(&UsartRx_Info[5]))
	{
		case 0x0001:
		{
			game_status.game_progress=((UsartRx_Info[7]>>4));
		}break;
		
		case 0x0101://bit10前哨战状态
		{
			event_data.event_type=(UsartRx_Info[8]<<5)>>7;
		}break;
		
		case 0x0201:
		{
			game_robot_state.robot_id                        =  UsartRx_Info[7];
			game_robot_state.robot_level                     =  UsartRx_Info[8];
			game_robot_state.remain_HP                       =  bit8TObit16(&UsartRx_Info[9]);
			game_robot_state.max_HP                          =  bit8TObit16(&UsartRx_Info[11]);
			game_robot_state.shooter_id1_17mm_cooling_rate      =  bit8TObit16(&UsartRx_Info[13]);
			game_robot_state.shooter_id1_17mm_cooling_limit     =  bit8TObit16(&UsartRx_Info[15]);
			game_robot_state.shooter_id1_17mm_speed_limit      =  bit8TObit16(&UsartRx_Info[17]);
			game_robot_state.shooter_id2_17mm_cooling_rate     =  bit8TObit16(&UsartRx_Info[19]);
			game_robot_state.shooter_id2_17mm_cooling_limit       =  bit8TObit16(&UsartRx_Info[21]);
			game_robot_state.shooter_id2_17mm_speed_limit      =  bit8TObit16(&UsartRx_Info[23]);
			game_robot_state.shooter_id1_42mm_cooling_rate      =  bit8TObit16(&UsartRx_Info[25]);
			game_robot_state.shooter_id1_42mm_cooling_limit      =  bit8TObit16(&UsartRx_Info[27]);
			game_robot_state.shooter_id1_42mm_speed_limit      =  bit8TObit16(&UsartRx_Info[29]);
			game_robot_state.chassis_power_limit      =  bit8TObit16(&UsartRx_Info[31]);
			
		}break;
		case 0x0202:
		{
			power_heat_data.chassis_volt               =  bit8TObit16(&UsartRx_Info[7]);
			power_heat_data.chassis_current            =  bit8TObit16(&UsartRx_Info[9]);
			power_heat_data.chassis_power              =  bit8TObit32(&UsartRx_Info[11]);//底盘功率
			power_heat_data.chassis_power_buffer       =  bit8TObit16(&UsartRx_Info[15]);//底盘功率缓冲
			power_heat_data.shooter_id1_17mm_cooling_heat              =  bit8TObit16(&UsartRx_Info[17]);
			power_heat_data.shooter_id2_17mm_cooling_heat              =  bit8TObit16(&UsartRx_Info[19]);			
			power_heat_data.shooter_id1_42mm_cooling_heat              =  bit8TObit16(&UsartRx_Info[21]);	//枪口热量	

		}break;
		case 0x0204:
		{
		  buff_data.power_rune_buff  =  UsartRx_Info	[7];
		}break;
		case 0x0206:
		{
//			armor_state_data.armor_id =  UsartRx_Info	[7]>>4;
//			armor_state_data.hurt_type =  UsartRx_Info	[7]&15;
			armor_state_data.armor_id =  (UsartRx_Info	[7]<<4)>>4;//前0后1
//			armor_state_data.hurt_type =  UsartRx_Info	[7]&15;
		}break;
		
		case 0x0207:
		{
		  real_shoot_data.bullet_speed=bit8TObit32(&UsartRx_Info[10]);
		}break;
		case 0x0208:
		{
			
		  bullet_remaining.bullet_remaining_num_17mm=bit8TObit16(&UsartRx_Info[7]);
		}break;
		default:
		{
		}	
	
	
	}

}

