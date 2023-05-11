#include "cmsis_os.h"
#include "gimbalTask.h"
#include "fireTask.h"
#include "bsp_rc.h"
#include "bsp_can.h"
#include "coverTask.h"
#include "vision.h"

uint8_t debug_mode=0;
extern uint8_t visiontast;

//״̬����_���޸�
void stateTask(void const * argument)
{
	rc_ctrl.online_Cnt = 250;
	TickType_t systick = 0;
	static uint8_t rc_s_flag[3][3];

    while (1)
    {
        systick = xTaskGetTickCount();
        if(systick -rc_ctrl.online_Cnt>50){//���ӳ�ʱ
            if(!debug_mode){
                gimbal.working = 0;
                gun.working = 0;
                coverSwitch = 0;
				visiontast=0;
                mcu_CAN.gimbal.dr16.chassisWork = 0;
                mcu_CAN.gimbal.dr16.chassisMode=0;
				for(int i=0;i<3;i++){
					for(int j=0;j<3;j++){
						rc_s_flag[i][j] = 0;
					}
				}
            }
        }
        else
        {
            
    /*���̼��*/
        /*�Ӿ�ʶ��Ŀ���л�*/
            static uint8_t flag_x = 0;
            if(rc_ctrl.key.set.X &&flag_x==0)
            {
                flag_x = 1;
                tx2.enemyMode = (tx2.enemyMode+1)%3;
            }
            if(rc_ctrl.key.set.X==0)
            {
                flag_x =  0;
            }
    
            /*���ݿ���*/
            static uint8_t flag_b = 0;
            static uint32_t capswitchtime;
            if(rc_ctrl.key.set.B &&flag_b==0)
            {
                flag_b = 1;
                capswitchtime = xTaskGetTickCount();
                mcu_CAN.gimbal.dr16.supercapWork = !mcu_CAN.gimbal.dr16.supercapWork;
            }
            if(rc_ctrl.key.set.B==0&&xTaskGetTickCount() - capswitchtime>5000)
            {
                flag_b = 0;
            }

            /*����ģʽ�л�*/
            static uint8_t flag_q = 0;
            if(rc_ctrl.key.set.Q &&flag_q==0)
            {
                flag_q = 1;
                gun.mode = !gun.mode;
            }
            if(rc_ctrl.key.set.Q==0)
            {
                flag_q =  0;
            }

            /*Ħ���ּ���*/
            static uint8_t flag_c = 0;
            if(rc_ctrl.key.set.C&&flag_c==0)
            {
                flag_c = 1;
                gun.frictionWheelSpeed[gun.bulletSpeed] -= 50;
            }
            if(rc_ctrl.key.set.C==0)
            {
                flag_c =  0;
            }

            /*Ħ���ּ���*/
            static uint8_t flag_f = 0;
            if(rc_ctrl.key.set.F&&flag_f==0)
            {
                flag_f = 1;
                gun.frictionWheelSpeed[gun.bulletSpeed] += 30;
            }
            if(rc_ctrl.key.set.F==0)
            {
                flag_f =  0;
            }

            /*����ģʽ�л�*/
            mcu_CAN.gimbal.dr16.chassisMode = 0;
            if(rc_ctrl.key.set.CTRL)    {mcu_CAN.gimbal.dr16.chassisMode = 1;}
            if(rc_ctrl.key.set.E)       {mcu_CAN.gimbal.dr16.chassisMode = 2;}
            if(rc_ctrl.key.set.SHIFT)   {mcu_CAN.gimbal.dr16.chassisMode = 4;}

    /*ң��������*/
            switch (rc_ctrl.rc.s[0])//�ҵ�ѡ��
            {
/*------------------------------------------------*/ 
            case 1://����
                switch (rc_ctrl.rc.s[1])//��ѡ��
                {
                case 1://����
					if(rc_s_flag[0][0] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[0][0] = 1;
                        gimbal.working = 1;
                        gun.working = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 1;
					}break;
                
                case 3://����
					if(rc_s_flag[1][0] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[1][0] = 1;
                        gimbal.working = 1;
                        gun.working = 1;
                        coverSwitch = 0;
						mcu_CAN.gimbal.dr16.chassisWork = 1;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                case 2://����
					if(rc_s_flag[2][0] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[2][0] = 1;
                        gimbal.working = 1;
                        gun.working = 1;
                        coverSwitch = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 1;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                default:
                        break;
                }
                break;
/*------------------------------------------------*/                    
            case 3://����
				if(tx2.enemyMode!=0)    {mcu_CAN.gimbal.dr16.chassisMode = 5;}
                switch (rc_ctrl.rc.s[1])//��ѡ��
                {
                case 1://����
					if(rc_s_flag[0][1] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[0][1] = 1;
                        gimbal.working = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 0;
                        gun.working = 1;
                        coverSwitch = 1;
						visiontast=0;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                case 3://����
					if(rc_s_flag[1][1] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[1][1] = 1;
                        gimbal.working = 0;
                        gun.working = 0;
                        coverSwitch = 0;
						visiontast=0;
						mcu_CAN.gimbal.dr16.chassisWork = 0;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                case 2://����
					if(rc_s_flag[2][1] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[2][1] = 1;
                        gimbal.working = 1;
                        gun.working = 1;
                        coverSwitch = 0;
						visiontast=1;
						mcu_CAN.gimbal.dr16.chassisWork = 0;
                        mcu_CAN.gimbal.dr16.chassisMode=0;
					}break;
                
                default:
                        break;
                }
            break;
/*------------------------------------------------*/                    
            case 2://����
                switch (rc_ctrl.rc.s[1])//��ѡ��
                {
                case 1://����
                    if(rc_s_flag[0][2] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[0][2] = 1;
                        gimbal.working = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 0;
                        gun.working = 1;
                        coverSwitch = 0;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                case 3://����
                    if(rc_s_flag[1][2] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[1][2] = 1;
                        gimbal.working = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 0;
                        gun.working = 0;
                        coverSwitch = 1;
                        mcu_CAN.gimbal.dr16.chassisMode = 0;
					}break;
                
                case 2://����
                    if(rc_s_flag[2][2] == 0){
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
								rc_s_flag[i][j] = 0;
							}
						}
						rc_s_flag[2][2] = 1;
                        gimbal.working = 1;
						mcu_CAN.gimbal.dr16.chassisWork = 1;
                        gun.working = 0;
                        coverSwitch = 0;
                        mcu_CAN.gimbal.dr16.chassisMode=0;
                        mcu_CAN.gimbal.dr16.supercapWork = 0;
					}break;
                
                default:
                        break;
                }
            break;

            default:
                    break;
            }
        }
    /*״̬���*/
        if(ABS(gun.fricL->Data.velocity)>2600||ABS(gun.fricR->Data.velocity)>2600){
            mcu_CAN.gimbal.dr16.allState |= 0x01;
        }else{
            mcu_CAN.gimbal.dr16.allState &= 0xfe;
        }
        if(tx2.enemyMode==0){
            mcu_CAN.gimbal.dr16.allState |= 0x02;
        }else{
            mcu_CAN.gimbal.dr16.allState &= 0xfd;
        }
        if(tx2.enemyMode==1){//С��
            mcu_CAN.gimbal.dr16.allState |= 0x04;
        }else{
            mcu_CAN.gimbal.dr16.allState &= 0xfb;
        }
        if(tx2.enemyMode==2){//���
            mcu_CAN.gimbal.dr16.allState |= 0x08;
        }else{
            mcu_CAN.gimbal.dr16.allState &= 0xf7;
        }
        
        vTaskDelay(2);
//		osDelay(2);

    }
}


