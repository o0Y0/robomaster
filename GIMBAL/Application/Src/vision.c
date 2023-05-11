#include "vision.h"
#include "cmsis_os.h"
#include "pid.h"


TX2 tx2={
    .isFind=0,
    .color_Flag = 0,//蓝色0绿色1红色2
    .kf_Flag = 0,
};
float yaw_signed,pit_signed;
float pit_unsigned =0;
float yaw_unsigned=0;
TickType_t systick = 0;
uint8_t vision_fire = 0.f;
uint8_t sum_check=0;
float distance;
int jjj;
void Vision_Check_Data(uint8_t *ReadFromUsart)
{
    if(ReadFromUsart[0]==0xAA&&ReadFromUsart[11]==0x54)
	{
		//加和校验
		sum_check = ReadFromUsart[1]+ReadFromUsart[2]+ReadFromUsart[3]+ReadFromUsart[4]
				  + ReadFromUsart[5]+ReadFromUsart[6]+ReadFromUsart[7];
		if(sum_check!=ReadFromUsart[8]) return;
		//识别到目标
		if(ReadFromUsart[1]==1)
		{
			systick = osKernelSysTick();
			tx2.isFind=1; 
			pit_unsigned = ReadFromUsart[4] + ReadFromUsart[5]*0.01f;
			yaw_unsigned = ReadFromUsart[2] + ReadFromUsart[3]*0.01f;

            switch (ReadFromUsart[6])
            {
            case 1://++
                pit_signed = -pit_unsigned;
                yaw_signed = -yaw_unsigned;
                break;
            case 2://+-
                pit_signed = -pit_unsigned;
                yaw_signed = +yaw_unsigned;
                break;
            case 3://--
                pit_signed = +pit_unsigned;
                yaw_signed = +yaw_unsigned;
                break;
            case 4://-+
                pit_signed = +pit_unsigned;
                yaw_signed = -yaw_unsigned;
                break;
            default:
                break;
            }
            tx2.yaw_Err[0] = yaw_signed;
            tx2.pit_Err[0] = pit_signed;
            VAL_Limit(tx2.yaw_Err[0],-2.f,2.f);
            VAL_Limit(tx2.yaw_Err[1],-3.f,3.f);
        }
        else
		{
			tx2.isFind = 0;
			tx2.yaw_Err[0] = 0;
			tx2.pit_Err[0] = 0;
		}
        tx2.isUpdata=1;//视觉数据更新完成
    }
}

