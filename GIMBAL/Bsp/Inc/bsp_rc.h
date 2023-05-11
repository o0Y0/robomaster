//
// Created by YanYuanbin on 22-10-3.
//

#ifndef BSP_RC_H
#define BSP_RC_H

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx.h"


#define SBUS_RX_BUF_NUM     36u
#define RC_FRAME_LENGTH     18u
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)

typedef  struct
{
    struct
    {
        int16_t ch[5];
        char s[2];
    } rc;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    } mouse;
    union
    {
        uint16_t v;
        struct
        {
            uint16_t W:1;
            uint16_t S:1;
            uint16_t A:1;
            uint16_t D:1;
            uint16_t SHIFT:1;
            uint16_t CTRL:1;
            uint16_t Q:1;
            uint16_t E:1;
            uint16_t R:1;
            uint16_t F:1;
            uint16_t G:1;
            uint16_t Z:1;
            uint16_t X:1;
            uint16_t C:1;
            uint16_t V:1;
            uint16_t B:1;
        }set;
    }key;
		
		bool rc_lost;
		uint32_t online_Cnt;
}rc_ctrl_t;

extern rc_ctrl_t rc_ctrl;

/* Exported functions --------------------------------------------------------*/
extern void remote_control_init(void);
extern void Remote_Control_RxEvent(UART_HandleTypeDef *huart);

extern void USAR_UART3_IDLECallback(UART_HandleTypeDef *huart);

#endif //BSP_RC_H
