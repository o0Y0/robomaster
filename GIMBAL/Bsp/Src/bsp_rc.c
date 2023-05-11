//
// Created by YanYuanbin on 22-10-3.
//
#include "bsp_rc.h"
#include "bsp_uart.h"
#include "cmsis_os.h"

rc_ctrl_t rc_ctrl;
static uint8_t SBUS_RX_BUF[2][SBUS_RX_BUF_NUM];

static void rc_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num)
{
    //enable the DMA transfer for the receiver request
    //使能DMA串口接收
    SET_BIT(huart3.Instance->CR3, USART_CR3_DMAR);

    //enalbe idle interrupt
    //使能空闲中断
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);

    //disable DMA
    //失效DMA    
    do{
        __HAL_DMA_DISABLE(huart3.hdmarx);
    }while(huart3.hdmarx->Instance->CR & DMA_SxCR_EN);

    huart3.hdmarx->Instance->PAR = (uint32_t) & (USART3->DR);
    //memory buffer 1
    //内存缓冲区1
    huart3.hdmarx->Instance->M0AR = (uint32_t)(rx1_buf);
    //memory buffer 2
    //内存缓冲区2
    huart3.hdmarx->Instance->M1AR = (uint32_t)(rx2_buf);
    //data length
    //数据长度
    huart3.hdmarx->Instance->NDTR = dma_buf_num;
    //enable double memory buffer
    //使能双缓冲区
    SET_BIT(huart3.hdmarx->Instance->CR, DMA_SxCR_DBM);

    //enable DMA
    //使能DMA
    __HAL_DMA_ENABLE(huart3.hdmarx);
}

void remote_control_init(void)
{
	rc_init(SBUS_RX_BUF[0], SBUS_RX_BUF[1], SBUS_RX_BUF_NUM);
}

/**
  * @brief  Transform the DMA data to remote control infomation.
  * @param  *sbus_buf   The source memory Buffer address  
  * @param  *rc_ctrl    pointer to a rc_ctrl_t structure
  * @retval NULL
  */
static void sbus_to_rc(volatile const uint8_t *sbus_buf, rc_ctrl_t  *rc_ctrl)
{
    if (sbus_buf == NULL || rc_ctrl == NULL) return;

    /* Channel 0, 1, 2, 3 */
    rc_ctrl->rc.ch[0] = (  sbus_buf[0]       | (sbus_buf[1] << 8 ) ) & 0x07ff;                            //!< Channel 0
    rc_ctrl->rc.ch[1] = ( (sbus_buf[1] >> 3) | (sbus_buf[2] << 5 ) ) & 0x07ff;                            //!< Channel 1
    rc_ctrl->rc.ch[2] = ( (sbus_buf[2] >> 6) | (sbus_buf[3] << 2 ) | (sbus_buf[4] << 10) ) & 0x07ff;      //!< Channel 2
    rc_ctrl->rc.ch[3] = ( (sbus_buf[4] >> 1) | (sbus_buf[5] << 7 ) ) & 0x07ff;                            //!< Channel 3
    rc_ctrl->rc.ch[4] = (  sbus_buf[16] 	 | (sbus_buf[17] << 8) ) & 0x07ff;                 			  //!< Channel 4
    /* Switch left, right */
    rc_ctrl->rc.s[0] = ((sbus_buf[5] >> 4) & 0x0003);                  //!< Switch left
    rc_ctrl->rc.s[1] = ((sbus_buf[5] >> 4) & 0x000C) >> 2;             //!< Switch right

    /* Mouse axis: X, Y, Z */
    rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);                    //!< Mouse X axis
    rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);                    //!< Mouse Y axis
    rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);                  //!< Mouse Z axis

    /* Mouse Left, Right Is Press  */
    rc_ctrl->mouse.press_l = sbus_buf[12];                                  //!< Mouse Left Is Press
    rc_ctrl->mouse.press_r = sbus_buf[13];                                  //!< Mouse Right Is Press

    /* KeyBoard value */
    rc_ctrl->key.v = sbus_buf[14] | (sbus_buf[15] << 8);                    //!< KeyBoard value

    rc_ctrl->rc.ch[0] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[1] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[2] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[3] -= RC_CH_VALUE_OFFSET;
    rc_ctrl->rc.ch[4] -= RC_CH_VALUE_OFFSET;
		
}

 void USAR_UART3_IDLECallback(UART_HandleTypeDef *huart)
{
	uint16_t this_time_rx_len = 0;
	rc_ctrl.online_Cnt = xTaskGetTickCount();

    /* Current memory buffer used is Memory 0 */
    if(((((DMA_Stream_TypeDef  *)huart->hdmarx->Instance)->CR) & DMA_SxCR_CT )== 0U)
    {
        //失能DMA中断
        __HAL_DMA_DISABLE(huart->hdmarx);
        //得到当前剩余数据长度
        this_time_rx_len = SBUS_RX_BUF_NUM - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        __HAL_DMA_SET_COUNTER(huart->hdmarx,SBUS_RX_BUF_NUM);
        huart->hdmarx->Instance->CR |= DMA_SxCR_CT;
        __HAL_DMA_ENABLE(huart->hdmarx);
        if(this_time_rx_len == RC_FRAME_LENGTH)
        {
            //处理遥控器数据
            sbus_to_rc(SBUS_RX_BUF[0], &rc_ctrl);
        }
    }else
    {
        //失能DMA中断
        __HAL_DMA_DISABLE(huart->hdmarx);
        //得到当前剩余数据长度
        this_time_rx_len = SBUS_RX_BUF_NUM - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        __HAL_DMA_SET_COUNTER(huart->hdmarx,SBUS_RX_BUF_NUM);
        huart->hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        __HAL_DMA_ENABLE(huart->hdmarx);
        if(this_time_rx_len == RC_FRAME_LENGTH)
        {
            //处理遥控器数据
            sbus_to_rc(SBUS_RX_BUF[1], &rc_ctrl);
        }
    }
}

