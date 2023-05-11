//
// Created by YanYuanbin on 23-3-30.
//
#include "message.h"

#include "BMI088_Driver.h"
//#include "IST8310_Driver.h"

#include "motor.h"


/* INS Message Function-------------------------------------------------------------*/
/**
  * @brief  transform the bmi088 receive data
  * @param  NULL
  * @retval bmi088 message memory address
  */
float const *f_get_bmi088_message(void)
{
		bmi088_real_data_t bmi088_read_data={0,};
		static float message[7]={0,};
		
		//bmi088 read data
		BMI088_read(bmi088_read_data.gyro, bmi088_read_data.accel, &bmi088_read_data.temp);
		
		//transform the message data
		for(uint8_t i=0;i<=2;i++)
		{
			message[i]  = bmi088_read_data.accel[i];
			message[i+3]= bmi088_read_data.gyro[i];
		}
		message[6] = bmi088_read_data.temp;

		return message;
}

/**
  * @brief  transform the ist8310 receive data
  * @param  NULL
  * @retval ist8310 message memory address
  */
//float const *f_get_ist8310_data(void)
//{
//	ist8310_real_data_t ist8310_read_data={0,};
//	static float message[3] = {0,};
//	
//	return message;
//}
