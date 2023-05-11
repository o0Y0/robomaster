#include "UI.h"
#include "bsp_usart.h"
#include "referee_info.h"
#include "move.h"


#define Robot_receiver_ID  (game_robot_state.robot_id | 0x100)  //�ͻ���
#define Robot_Sentry_ID    (uint16_t)(107)  //��ɫ����  100  �ڱ�ID 7

uint32_t division_value = 10;

UI_Info_t UI;
int update_figure_flag,update_aim_flag,update_float_flag,update_supercapacitor_flag,update_int_flag;
bool global_fri,global_spin,global_auto_aim,global_auto;//ȦȦ

float global_supercapacitor_remain = 77.3f;//[0,100]

uint32_t global_sight_bead_x = 960,global_sight_bead_y = 720,global_supercapacitor_point=20;//[0,100]

/**
  * @brief  ����һ�η���һ���ֽ�����
  * @param  �Լ�����õ�Ҫ�������е�����
  * @retval void
  * @attention  ������λ����
  */
//void UART8_SendChar(uint8_t cData)
//{
//	while (USART_GetFlagStatus( USART6, USART_FLAG_TC ) == RESET);
//	
//	USART_SendData( USART6, cData );   
//}

ext_charstring_data_t tx_client_char;
uint8_t CliendTxBuffer[200];

ext_interact_data_t tx_Sentry_data;
uint8_t SentryTxBuffer[200];  //���ڱ��������ݣ��������� 

uint8_t state_first_graphic = 0 ;//0~7ѭ��
char first_line[30]  = {" FRI:"};//�Ƿ�������,����30���ַ�����bool
char third_line[30]  = {"AUTO:"};//����
char empty_line[30]  = {"                   "};


//����ͨ��_���͸��ڱ�����
void Tx_To_Sentry(int on)
{
		//֡ͷ
		tx_Sentry_data.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		tx_Sentry_data.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(ext_interact_data_t);
		tx_Sentry_data.txFrameHeader.Seq = 0;//�����
		memcpy(SentryTxBuffer,&tx_Sentry_data.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(SentryTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		tx_Sentry_data.CmdID = 0x0301;

		//���ݶ�ͷ�ṹ
		tx_Sentry_data.dataFrameHeader.data_cmd_id = INTERACT_ID_Tx_To_Sentry_Data;
		tx_Sentry_data.dataFrameHeader.send_ID     = game_robot_state.robot_id;
		tx_Sentry_data.dataFrameHeader.receiver_ID = Robot_Sentry_ID;
	
		//���ݶ�
//		Tx_Sentry_Data();
	  if(on){
			tx_Sentry_data.clientData.data[0] = 1;/*Key_W      | 
																					Key_S<<1   | 
																					Key_A<<2   | 
																					Key_D<<3   |
																					Key_CTRL<<4;*/
//																				Key_CTRL<<4				
	  } else tx_Sentry_data.clientData.data[0] =0;
		
//		memcpy(&tx_Sentry_data.clientData,empty_line,19);	
		

		memcpy(SentryTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_Sentry_data.CmdID, LEN_CMD_ID+tx_Sentry_data.txFrameHeader.DataLength);//���������볤��2
		
		//֡β
		Append_CRC16_Check_Sum(SentryTxBuffer,sizeof(tx_Sentry_data));
		
    for(int i = 0; i < sizeof(tx_Sentry_data); i++) 
      UART8_SendChar(SentryTxBuffer[i]);
}





















void Char_Graphic(ext_client_string_t* graphic,//����Ҫ����ȥ�������е����ݶ�����
									const char* name,
									uint32_t operate_tpye,
									
									uint32_t layer,
									uint32_t color,
									uint32_t size,
									uint32_t length,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,

									
									
									const char *character)//�ⲿ���������
{
	graphic_data_struct_t *data_struct = &graphic->grapic_data_struct;
	for(char i=0;i<3;i++)
		data_struct->graphic_name[i] = name[i];	//�ַ�����
	data_struct->operate_tpye = operate_tpye; //ͼ�����
	data_struct->graphic_tpye = CHAR;         //Char��
	data_struct->layer = layer;//���ڵ����
	data_struct->color = color;//���ǰ�ɫ
	data_struct->start_angle = size;
	data_struct->end_angle = length;	
	data_struct->width = width;
	data_struct->start_x = start_x;
	data_struct->start_y = start_y;	
	
	data_struct->radius = 0;
	data_struct->end_x = 0;
	data_struct->end_y = 0;
	memcpy(graphic->data,empty_line,19);
    memcpy(graphic->data,character,length);
}


static void Draw_char()
{
	if(state_first_graphic == 0)//��֪��ʲôʱ�����ͻ�������Ҫ���ϸ���
	{
		Char_Graphic(&tx_client_char.clientData,"CL1",ADD,0,YELLOW,15,strlen(first_line),2,(45),(1080*16/20),first_line);//x1920/18
		state_first_graphic = 1;
	}
	else if(state_first_graphic == 1)//��֪��ʲôʱ�����ͻ�������Ҫ���ϸ���
	{
		Char_Graphic(&tx_client_char.clientData,"CL2",ADD,0,YELLOW,15,strlen(third_line),2,(45),(1080*14/20),third_line);//x1920/18
		state_first_graphic = 3;
	}

}
void Client_graphic_Init(void)
{
	if(state_first_graphic>=2)
	{
		state_first_graphic = 0;
	}
		//֡ͷ
		tx_client_char.txFrameHeader.SOF = 0xA5;
		tx_client_char.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(ext_client_string_t);
		tx_client_char.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_client_char.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��
	
		//������
		tx_client_char.CmdID = 0x0301;
		
		//���ݶ�ͷ�ṹ
		tx_client_char.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_char_graphic;
		tx_client_char.dataFrameHeader.send_ID     = game_robot_state.robot_id;
		tx_client_char.dataFrameHeader.receiver_ID = Robot_receiver_ID;

		
		//���ݶ�
		Draw_char();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_char.CmdID, LEN_CMD_ID+tx_client_char.txFrameHeader.DataLength);//���������볤��2
		
		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_char));
		
    for(int i = 0; i < sizeof(tx_client_char); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
bool IF_Init_Over = false;
void UI_Init(void)
{
	if(!IF_Init_Over)
	{
	update_figure_flag = ADD;
	update_aim_flag = ADD;
	update_float_flag = ADD;
	update_supercapacitor_flag = ADD;
	update_int_flag = ADD;
	IF_Init_Over = true;
	}
}
void UI_DataUpdate(void)
{
	UI.User.SPIN = (bool)(chassis.workMode == 1);
	UI.User.Vcap_show = chassis.cap_V;
}
void Figure_INT(Int_data_struct_t *Int,
					  const char* name,
					  uint32_t operate_tpye,
					  uint32_t graphic_tpye,
					  uint32_t layer,
					  uint32_t color,
					  uint32_t start_angle,
					  uint32_t end_angle,
					  uint32_t width,
					  uint32_t start_x,
					  uint32_t start_y,
					  int number
								)
{
	for(char i=0;i<3;i++)
	Int->graphic_name[i] = name[i];
	Int->operate_tpye = operate_tpye;
	Int->graphic_tpye = graphic_tpye;
	Int->layer = layer;
	Int->color = color;
	Int->start_angle = start_angle;
	Int->end_angle = end_angle;
	Int->width = width;
	Int->start_x = start_x;
	Int->start_y = start_y;
	Int->number = number;
}
void Figure_Graphic(graphic_data_struct_t* graphic,//����Ҫ����ȥ����������ݶ�����
									const char* name,
									uint32_t operate_tpye,
									uint32_t graphic_tpye,//����ʲôͼ��
									uint32_t layer,
									uint32_t color,
									uint32_t start_angle,
									uint32_t end_angle,
									uint32_t width,
									uint32_t start_x,
									uint32_t start_y,
									uint32_t radius,
									uint32_t end_x,
									uint32_t end_y)							
{
	for(char i=0;i<3;i++)
	graphic->graphic_name[i] = name[i];	//�ַ�����
	graphic->operate_tpye = operate_tpye; //ͼ�����
	graphic->graphic_tpye = graphic_tpye;         //Char��
	graphic->layer        = layer;//���ڵ�һ��
	graphic->color        = color;//��ɫ
	graphic->start_angle  = start_angle;
	graphic->end_angle    = end_angle;	
	graphic->width        = width;
	graphic->start_x      = start_x;
	graphic->start_y      = start_y;	
	graphic->radius = radius;
	graphic->end_x  = end_x;
	graphic->end_y  = end_y;
}
//************************************��������*******************************/
ext_graphic_seven_data_t tx_client_graphic_figure;
static void fri_figure(void)//Ħ���ִ�Ϊtrue
{
	if(mcu_CAN.gimbal.dr16.allState&0x01)//��׼�����Ϊ��ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,GREEN,  0,0,5, 130,1080*16/20-9, 15,0,0);
	else //Ħ���ֹر�
		Figure_Graphic(&tx_client_graphic_figure.clientData[0],"GL1",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5, 130,1080*16/20-9, 15,0,0);
}
static void auto_figure(void)//�쳣
{
	if((mcu_CAN.gimbal.dr16.allState&0x02) == 2)//�Ե���ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[2],"GL3",update_figure_flag,CIRCLE,1,GREEN,0,0,5,   130,1080*14/20-9, 15,0,0);
	if((mcu_CAN.gimbal.dr16.allState&0x04) == 4)//С����ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[2],"GL3",update_figure_flag,CIRCLE,1,ORANGE,0,0,5, 130,1080*14/20-9, 15,0,0);
    if((mcu_CAN.gimbal.dr16.allState&0x08) == 8)//�����ɫ
		Figure_Graphic(&tx_client_graphic_figure.clientData[2],"GL3",update_figure_flag,CIRCLE,1,FUCHSIA,0,0,5, 130,1080*14/20-9, 15,0,0);
}
static void Draw_Figure_bool()
{
	fri_figure();
	auto_figure();
}
void Client_graphic_Info_update()//�߸�ͼ��һ�����
{
		//֡ͷ
		tx_client_graphic_figure.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		tx_client_graphic_figure.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
		tx_client_graphic_figure.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_client_graphic_figure.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		tx_client_graphic_figure.CmdID = 0x0301;

		//���ݶ�ͷ�ṹ
		tx_client_graphic_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		tx_client_graphic_figure.dataFrameHeader.send_ID     = game_robot_state.robot_id;
		tx_client_graphic_figure.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		Draw_Figure_bool();
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_client_graphic_figure.CmdID, LEN_CMD_ID+tx_client_graphic_figure.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_client_graphic_figure));
		
    for(int i = 0; i < sizeof(tx_client_graphic_figure); i++) 
		{
      UART8_SendChar(CliendTxBuffer[i]);
    }

}
ext_graphic_seven_data_t high_aim_figure;//������׼��֮��,������
ext_graphic_seven_data_t low_aim_shortfigure_1;//׼���µĵ�һ������
ext_graphic_seven_data_t low_aim_shortfigure_2;
ext_graphic_seven_data_t low_aim_shortfigure_3;//���������������
ext_graphic_seven_data_t low_aim_longfigure;//׼���µĳ�����

//stem--��
static void aim_lowshort_stem(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ���"AM"--aim_low_bottom,"AS"--aim_stem
{ 
	Figure_Graphic(&low_aim_shortfigure_3.clientData[0],"AB1",ADD,LINE,3,YELLOW,0,0,1,   960-line_length,540-30-division_value*18,0,   960+line_length,540-30-division_value*18);//graphic_Remove
	Figure_Graphic(&low_aim_shortfigure_3.clientData[1],"AB2",ADD,LINE,3,YELLOW,0,0,1,   960-line_length,540-30-division_value*20,0,   960+line_length,540-30-division_value*20);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[2],"AB3",ADD,LINE,3,YELLOW,0,0,1,   960-line_length,540-30-division_value*21,0,   960+line_length,540-30-division_value*21);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[3],"AB4",ADD,LINE,3,YELLOW,0,0,1,   960-line_length,540-30-division_value*22,0,   960+line_length,540-30-division_value*22);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[4],"AB5",ADD,LINE,3,YELLOW,0,0,1,960-line_length-10,540-30-division_value*23,0,960+line_length+10,540-30-division_value*23);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[5],"AS1",ADD,LINE,3,YELLOW,0,0,1,   960,            540+30+division_value*6 ,0,   960,            540+30);
	Figure_Graphic(&low_aim_shortfigure_3.clientData[6],"AS2",ADD,LINE,3,YELLOW,0,0,1,   960,            540-30-division_value*23,0,   960,            540-30);

}
void _lowshortstem_aim_4()
{
		//֡ͷ
		low_aim_shortfigure_3.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_3.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
		low_aim_shortfigure_3.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_3.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		low_aim_shortfigure_3.CmdID = 0x0301;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_3.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		low_aim_shortfigure_3.dataFrameHeader.send_ID     = game_robot_state.robot_id;
		low_aim_shortfigure_3.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		aim_lowshort_stem(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_3.CmdID, LEN_CMD_ID+low_aim_shortfigure_3.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_3));
		
    for(int i = 0; i < sizeof(low_aim_shortfigure_3); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
//ͼ����
static void aim_lowlong(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ���"AM"--aim_low_Long,"AS"--aim_stem
{ 
	Figure_Graphic(&low_aim_longfigure.clientData[0],"AL1",ADD,LINE,4,YELLOW,0,0,1,960-line_length-30,540-30-division_value*19,0,960+line_length+30,540-30-division_value*19);//graphic_Remove
	Figure_Graphic(&low_aim_longfigure.clientData[1],"AL2",ADD,LINE,4,YELLOW,0,0,1,960-line_length-40,540-30-division_value*15,0,960+line_length+40,540-30-division_value*15);
	Figure_Graphic(&low_aim_longfigure.clientData[2],"AL3",ADD,LINE,4,YELLOW,0,0,1,960-line_length-50,540-30-division_value*11,0,960+line_length+50,540-30-division_value*11);
	Figure_Graphic(&low_aim_longfigure.clientData[3],"AL4",ADD,LINE,4,YELLOW,0,0,1,960-line_length-60,540-30-division_value*7 ,0,960+line_length+60,540-30-division_value*7 );
	Figure_Graphic(&low_aim_longfigure.clientData[4],"AL5",ADD,LINE,4,YELLOW,0,0,1,960-line_length-70,540-30-division_value*3 ,0,960+line_length+70,540-30-division_value*3 );
	//����
	Figure_Graphic(&low_aim_longfigure.clientData[5],"AL6",ADD,LINE,4,YELLOW,0,0,3,540,0, 0,640,100);
	Figure_Graphic(&low_aim_longfigure.clientData[6],"AL7",ADD,LINE,4,YELLOW,0,0,3,1380,0, 0,1280,100);

}
void _lowlong_aim_()
{
	//֡ͷ
	low_aim_longfigure.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
	low_aim_longfigure.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
	low_aim_longfigure.txFrameHeader.Seq = 0;//�����
	memcpy(CliendTxBuffer,&low_aim_longfigure.txFrameHeader,sizeof(xFrameHeader));
	Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

	//������
	low_aim_longfigure.CmdID = 0x301;

	//���ݶ�ͷ�ṹ
	low_aim_longfigure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
	low_aim_longfigure.dataFrameHeader.send_ID     = game_robot_state.robot_id;
	low_aim_longfigure.dataFrameHeader.receiver_ID = Robot_receiver_ID;

	//���ݶ�
	aim_lowlong(division_value,10);
	memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_longfigure.CmdID, LEN_CMD_ID+low_aim_longfigure.txFrameHeader.DataLength);//���������볤��2

	//֡β
	Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_longfigure));
		
    for(int i = 0; i < sizeof(low_aim_longfigure); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
//ʣ�����ֻ��һ��ͼ��
ext_graphic_one_data_t tx_supercapacitor_figure;
int update_supercapacitor_flag;
static void supercapacitor_figure(float remain_energy,uint32_t turning_point)//ʣ�೬�����ݣ���λ�ٷֱȣ�������ĳ�ٷֱȱ��ɫ
{
	uint32_t remaining = (uint32_t)remain_energy;//ǿ��ת��
	
	if(remaining >= turning_point)//ֱ�߳���Ϊ3
		Figure_Graphic(&tx_supercapacitor_figure.clientData,"SR1",update_supercapacitor_flag,LINE,2,GREEN,0,0,10,670,20  ,0, 670+remaining*6,20);
	else if(remaining < turning_point)
		Figure_Graphic(&tx_supercapacitor_figure.clientData,"SR1",update_supercapacitor_flag,LINE,2,FUCHSIA,0,0,10,670,20  ,0,  670+remaining*6,20);
	}
void Client_supercapacitor_update()//һ��ͼ�����
{
		//֡ͷ
		tx_supercapacitor_figure.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		tx_supercapacitor_figure.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t);
		tx_supercapacitor_figure.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&tx_supercapacitor_figure.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		tx_supercapacitor_figure.CmdID = 0x301;

		//���ݶ�ͷ�ṹ
		tx_supercapacitor_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_one_graphic;
		tx_supercapacitor_figure.dataFrameHeader.send_ID     = game_robot_state.robot_id;;
		tx_supercapacitor_figure.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		supercapacitor_figure(global_supercapacitor_remain,global_supercapacitor_point);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&tx_supercapacitor_figure.CmdID, LEN_CMD_ID+tx_supercapacitor_figure.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(tx_supercapacitor_figure));
		
    for(int i = 0; i < sizeof(tx_supercapacitor_figure); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
//division_value�ֶ�ֵ,line_length���߳��ȵ�һ��
static void aim_1(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ���"AH"--aim_high
{
	Figure_Graphic(&high_aim_figure.clientData[0],"AH1",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30                 ,0,  960+line_length,540+30);//graphic_Remove
	Figure_Graphic(&high_aim_figure.clientData[1],"AH2",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value  ,0,  960+line_length,540+30+division_value  );
	Figure_Graphic(&high_aim_figure.clientData[2],"AH3",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value*2,0,  960+line_length,540+30+division_value*2);
	Figure_Graphic(&high_aim_figure.clientData[3],"AH4",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value*3,0,  960+line_length,540+30+division_value*3);
	Figure_Graphic(&high_aim_figure.clientData[4],"AH5",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value*4,0,  960+line_length,540+30+division_value*4);
	Figure_Graphic(&high_aim_figure.clientData[5],"AH6",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value*5,0,  960+line_length,540+30+division_value*5);
	Figure_Graphic(&high_aim_figure.clientData[6],"AH7",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540+30+division_value*6,0,  960+line_length,540+30+division_value*6);
}
void _high_aim_()
{
		//֡ͷ
		high_aim_figure.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		high_aim_figure.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
		high_aim_figure.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&high_aim_figure.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		high_aim_figure.CmdID = 0x301;

		//���ݶ�ͷ�ṹ
		high_aim_figure.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		high_aim_figure.dataFrameHeader.send_ID     = game_robot_state.robot_id;;
		high_aim_figure.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		aim_1(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&high_aim_figure.CmdID, LEN_CMD_ID+high_aim_figure.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(high_aim_figure));
		
    for(int i = 0; i < sizeof(high_aim_figure); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
static void aim_lowshort_2(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ���"AL"--aim_low
{
	Figure_Graphic(&low_aim_shortfigure_1.clientData[0],"AL1",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30                 ,0,  960+line_length,540-30);//graphic_Remove
	Figure_Graphic(&low_aim_shortfigure_1.clientData[1],"AL2",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value  ,0,  960+line_length,540-30-division_value  );
	Figure_Graphic(&low_aim_shortfigure_1.clientData[2],"AL3",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*2,0,  960+line_length,540-30-division_value*2);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[3],"AL4",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*4,0,  960+line_length,540-30-division_value*4);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[4],"AL5",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*5,0,  960+line_length,540-30-division_value*5);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[5],"AL6",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*6,0,  960+line_length,540-30-division_value*6);
	Figure_Graphic(&low_aim_shortfigure_1.clientData[6],"AL7",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*8,0,  960+line_length,540-30-division_value*8);
}
void _lowshort_aim_2()
{
		//֡ͷ
		low_aim_shortfigure_1.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_1.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
		low_aim_shortfigure_1.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_1.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		low_aim_shortfigure_1.CmdID = 0x301;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_1.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		low_aim_shortfigure_1.dataFrameHeader.send_ID     =	game_robot_state.robot_id;
		low_aim_shortfigure_1.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		aim_lowshort_2(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_1.CmdID, LEN_CMD_ID+low_aim_shortfigure_1.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_1));
		
    for(int i = 0; i < sizeof(low_aim_shortfigure_1); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
static void aim_lowshort_3(uint32_t division_value,uint32_t line_length)//׼���ϰ벿�ֵĿ���"AM"--aim_low_middle
{
	Figure_Graphic(&low_aim_shortfigure_2.clientData[0],"AM1",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*9 ,0,  960+line_length,540-30-division_value*9 );//graphic_Remove
	Figure_Graphic(&low_aim_shortfigure_2.clientData[1],"AM2",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*10,0,  960+line_length,540-30-division_value*10);
	Figure_Graphic(&low_aim_shortfigure_2.clientData[2],"AM3",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*12,0,  960+line_length,540-30-division_value*12);
	Figure_Graphic(&low_aim_shortfigure_2.clientData[3],"AM4",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*13,0,  960+line_length,540-30-division_value*13);
	Figure_Graphic(&low_aim_shortfigure_2.clientData[4],"AM5",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*14,0,  960+line_length,540-30-division_value*14);
	Figure_Graphic(&low_aim_shortfigure_2.clientData[5],"AM6",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*16,0,  960+line_length,540-30-division_value*16);
	Figure_Graphic(&low_aim_shortfigure_2.clientData[6],"AM7",ADD,LINE,3,YELLOW,0,0,1,  960-line_length,540-30-division_value*17,0,  960+line_length,540-30-division_value*17);
}
void _lowshort_aim_3()
{
		//֡ͷ
		low_aim_shortfigure_2.txFrameHeader.SOF = JUDGE_FRAME_HEADER;
		low_aim_shortfigure_2.txFrameHeader.DataLength = sizeof(ext_student_interactive_header_data_t) + sizeof(graphic_data_struct_t)*7;
		low_aim_shortfigure_2.txFrameHeader.Seq = 0;//�����
		memcpy(CliendTxBuffer,&low_aim_shortfigure_2.txFrameHeader,sizeof(xFrameHeader));
		Append_CRC8_Check_Sum(CliendTxBuffer, sizeof(xFrameHeader));//ͷУ��

		//������
		low_aim_shortfigure_2.CmdID = 0x301;

		//���ݶ�ͷ�ṹ
		low_aim_shortfigure_2.dataFrameHeader.data_cmd_id = INTERACT_ID_draw_seven_graphic;
		low_aim_shortfigure_2.dataFrameHeader.send_ID     = game_robot_state.robot_id;
		low_aim_shortfigure_2.dataFrameHeader.receiver_ID = Robot_receiver_ID;
	
		//���ݶ�
		aim_lowshort_3(division_value,10);
		memcpy(CliendTxBuffer+LEN_FRAME_HEAD, (uint8_t*)&low_aim_shortfigure_2.CmdID, LEN_CMD_ID+low_aim_shortfigure_2.txFrameHeader.DataLength);//���������볤��2

		//֡β
		Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(low_aim_shortfigure_2));
		
    for(int i = 0; i < sizeof(low_aim_shortfigure_2); i++) {
      UART8_SendChar(CliendTxBuffer[i]);
    }
}
bool IF_start_ui=1;

//�޸�_�������ڱ�����ͨ��
void Startjudge_task(void)
{	
  static int i = -1,J = 0;
  i++;
  J++;
  UI_Init();
  UI_DataUpdate();
  
//��⵽����B����,�����ڱ���, ռ�ô��� ��Ҳ���Ըĳ�ռ��ȫ��������
//	if(KEY_PRESSED_G)   //�Ұ��I�أ�
//w 
		Tx_To_Sentry(1); //�������
	
  switch(i)
  {
    case 0:
        Client_graphic_Init();   //����
        break;
    
    case 1:
        global_fri     = UI.User.FRI;
        global_spin     = UI.User.SPIN;
        Client_graphic_Info_update();//ԲȦ
        break;
    
	case 2:
        global_supercapacitor_remain = (UI.User.Vcap_show - 14.f)/(23.f-14.f) * 100.f;
		VAL_LIMIT(global_supercapacitor_remain,0,100);
        Client_supercapacitor_update();
        update_supercapacitor_flag = MODIFY;
	    update_figure_flag = MODIFY;
	break;

    default:
        i = -1;
        break;
  }  
  if(J == 100)
  {
    J = 0;
	IF_Init_Over = false;
  }
}