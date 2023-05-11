
//
// Created by YanYuanbin on 23-3-30.
//

#include "bsp_usart.h"
#include "bsp_rc.h"

/**
  * @brief  Starts the UART RxEventCallback .
  * @param  uartHandle pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART.  
  * @param  DataLength The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef UART_TOIDLE_Start(UART_HandleTypeDef* uartHandle,uint32_t DataLength)
{
		/* Check the UART handle allocation */
		if (uartHandle == NULL)return HAL_ERROR;
		
		/* Configure UART RxXferSize */
		uartHandle->RxXferSize = DataLength;
	
		/* reception Mode TOIDLE */
		uartHandle->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
	
		/* UART IDLE ENABLE */
		__HAL_UART_CLEAR_IDLEFLAG(uartHandle);
		__HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);

		/* ENABLE UART DMA Receive */
		SET_BIT(uartHandle->Instance->CR3, USART_CR3_DMAR);
		
		return HAL_OK;
}

/**
  * @brief  Starts the multi_buffer DMA Transfer.
  * @param  hdma       pointer to a DMA_HandleTypeDef structure that contains
  *                    the configuration information for the specified DMA Stream.  
  * @param  SrcAddress The source memory Buffer address
  * @param  DstAddress The destination memory Buffer address
  * @param  SecondMemAddress The second memory Buffer address in case of multi buffer Transfer  
  * @param  DataLength The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef DMA_MultiBufferStart(DMA_HandleTypeDef *hdma,uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{
	  HAL_StatusTypeDef status = HAL_OK;
	
		/* Memory-to-memory transfer not supported in double buffering mode */
		if (hdma->Init.Direction == DMA_MEMORY_TO_MEMORY)
		{
			hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
			status = HAL_ERROR;
		}else
		{
			if(hdma->State == HAL_DMA_STATE_READY )
			{
				/* Change DMA peripheral state */
				hdma->State = HAL_DMA_STATE_BUSY; 

				 /* DMA DISABLE */
				do{
						__HAL_DMA_DISABLE(hdma);
				}while(hdma->Instance->CR & DMA_SxCR_EN);

				  /* Peripheral to Memory */
				if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
				{   
					/* Configure DMA Stream destination address */
					hdma->Instance->PAR = DstAddress;
					
					/* Configure DMA Stream source address */
					hdma->Instance->M0AR = SrcAddress;
				}
				/* Memory to Peripheral */
				else
				{
					/* Configure DMA Stream source address */
					hdma->Instance->PAR = SrcAddress;
					
					/* Configure DMA Stream destination address */
					hdma->Instance->M0AR = DstAddress;
				}
				
				//memory buffer 2
				hdma->Instance->M1AR = SecondMemAddress;
				//data length
				hdma->Instance->NDTR = DataLength;
				
				//ʹ��˫������
				SET_BIT(hdma->Instance->CR, DMA_SxCR_DBM);
				/* DMA ENABLE */
				__HAL_DMA_ENABLE(hdma);
			}else
			{
					/* Return error status */
					status = HAL_BUSY;
			}
		}
		
		return status;
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    // �ж��Ƿ��Ǵ���1
    if(huart->Instance == USART1)
    {

    }
		// �ж��Ƿ��Ǵ���3
	else if(huart->Instance == USART3)
	{   // �ж��Ƿ��ǿ����ж�
		 if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
		{	 // ��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(huart);
            // �����жϴ�����
            USAR_UART3_IDLECallback(huart);
        }
	}
	else
	{
			return ;
	}
}
