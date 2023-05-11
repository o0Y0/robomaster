#ifndef VISION_H
#define VISION_H

#include "main.h"

#define VISUAL_RX_BUF_NUM 32u
#define VISUAL_FRAME_LENGTH 64u
#define VISION_BUFFER_LEN  10

typedef struct
{
    uint8_t isUpdata;
    uint8_t isFind;//0Ϊ��Ŀ��
    uint8_t color_Flag;//��ɫ1 ��ɫ2
    uint8_t kf_Flag;//�Ƿ��������� 0��1
    uint8_t enemyMode;//0�з���λ��1С����2���
    float yaw_Err[2];
	float pit_Err[2];
    int distance;
}TX2;
extern TX2 tx2;

extern float distance;
extern float yaw_signed,pit_signed;
extern float vision_Info[2];
extern float camera_final_YAW,camera_final_PITCH,camera_final_DISTANCE;
extern int16_t Re_buf[10],_sign,counter;
extern uint8_t Receive_Usart3_Buff[VISION_BUFFER_LEN];
extern uint8_t visual_rx_buf[2][VISUAL_RX_BUF_NUM];//�����������
extern uint8_t camer1[9];   


void Vision_Check_Data(uint8_t *ReadFromUsart);

#endif



