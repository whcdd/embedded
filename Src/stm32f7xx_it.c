/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      stm32f7xx_it.c                                                                                              ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   中断服务函数                                                                                                 ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/09/25                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_it.h"
#include "debug_uart5.h"
#include "DynamixelMotor.h"
#include "config.h"


unsigned char UART4RxEndFlag = 0;
unsigned int UART4RxLen = 0;
unsigned char UART1RxEndFlag = 0;
unsigned int UART1RxLen = 0;
unsigned char UART5RxEndFlag = 0;
unsigned int UART5RxLen = 0;

void NMI_Handler(void) 
{
  
  HAL_RCC_NMI_IRQHandler();
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
 
  while (1)
  {
  
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
    
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  
  while (1)
  {
    
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
 
  while (1)
  {
   
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  
  HAL_IncTick();
  
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
	u32 RxLen;
	volatile u32 a = 0;

	//  HAL_UART_IRQHandler(&huart1);
	if(__HAL_UART_GET_IT(&huart1,UART_IT_RTO))
	{
        __HAL_UART_CLEAR_IT(&huart1,UART_CLEAR_RTOF);
			
        /* Disable the DMA */
        __HAL_DMA_DISABLE(&hdma_uart1_rx);
		
        /* Wait until DMA_SXCR is 0 */
        while(READ_BIT(hdma_uart1_rx.Instance->CR,0)!=0);
		  
        /* Derive the receiving data length from the NDTR register */
        RxLen = DXL_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_uart1_rx);
		
        /* Clear DMA TCIF2& HTIF2 flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_uart1_rx,DMA_FLAG_TCIF2_6|DMA_FLAG_HTIF2_6|DMA_FLAG_TEIF2_6|DMA_FLAG_DMEIF2_6|DMA_FLAG_FEIF2_6);
		  
        /* Reset the DMA NDTR */
        __HAL_DMA_SET_COUNTER(&hdma_uart1_rx,DXL_RXBUFFSIZE);
		UART1RxLen = RxLen;
		UART1RxEndFlag = 1; //标志已经成功接收到一包等待处理
//		for(i = 0;i<RxLen;i++)
//		{
//			USB2UART_aTxBuffer0[i] = USB2UART_aRxBuffer0[i];
//		}
//		/* Progress the reveiving data */
//		USB2UART_SendData(USB2UART_aTxBuffer0,RxLen);
			
		/* Enable the DMA */
//		if((DXL_aRxBuffer[0] !=0xff)&&(DXL_aRxBuffer[1] !=0xff)&& (DXL_aRxBuffer[2] != 0xfd))
//		{
//		    a += 1;
//		}
        __HAL_DMA_ENABLE(&hdma_uart1_rx);		
	}
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
    u32 RxLen;
    if(__HAL_UART_GET_IT(&huart2,UART_IT_RTO))
    {
        __HAL_UART_CLEAR_IT(&huart2,UART_CLEAR_RTOF);
				
        /* Disable the DMA */
        __HAL_DMA_DISABLE(&hdma_usart2_rx);
			  
        /* Derive the receiving data length from the NDTR register */
        RxLen = MTi_630_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
			
        /* Clear DMA TCIF2& HTIF2 flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_usart2_rx,DMA_FLAG_TCIF1_5);
			  
        /* Reset the DMA NDTR */
        __HAL_DMA_SET_COUNTER(&hdma_usart2_rx,MTi_630_RXBUFFSIZE);
				
        /* Progress the reveiving data */
        USB2UART_SendData(MTi_630_aRxBuffer0,RxLen);
				
        /* Enable the DMA */
        __HAL_DMA_ENABLE(&hdma_usart2_rx);
				
    }
}

/*         MTi630 Interrupt Function        */
/**
  * @brief This function handles DMA1 stream5 hdma_usart2_rx global interrupt. MTi-630
  */
void DMA1_Stream5_IRQHandler(void)
{
	volatile u32 RxLen;
	if(__HAL_DMA_GET_FLAG(&hdma_usart2_rx,DMA_FLAG_TCIF1_5))
	{
        /* Clear DMA Transfer complete flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_usart2_rx,DMA_FLAG_TCIF1_5);
		
			  
        /* Derive the receiving data length from the NDTR register */
        RxLen = MTi_630_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);	
		  
        /* Progress the reveiving data -copy buffer content to MTi_630_aRxBuffer*/
        if(READ_BIT(hdma_usart2_rx.Instance->CR,DMA_SxCR_CT) == MTI630_BUFFER0)
        {
            MTi_630_aRxBuffer = MTi_630_aRxBuffer0;
//			copySrcBufferToDesMem(MTi_630_aRxBuffer0,MTi_630_aRxBuffer,MTi_630_RXBUFFSIZE);
//			USB2UART_SendData(MTi_630_aRxBuffer0,MTi_630_RXBUFFSIZE);
		}
		else
		{
			MTi_630_aRxBuffer = MTi_630_aRxBuffer1;
//		    copySrcBufferToDesMem(MTi_630_aRxBuffer1,MTi_630_aRxBuffer,MTi_630_RXBUFFSIZE);
//			USB2UART_SendData(MTi_630_aRxBuffer1,MTi_630_RXBUFFSIZE);
		}
	}
}


/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
 
}

/**
  * @brief This function handles UART4 global interrupt.   USB2UART
  */
void UART4_IRQHandler(void)
{
    u32 RxLen;
	volatile u32 a = 0;

//  HAL_UART_IRQHandler(&huart4);
	if(__HAL_UART_GET_IT(&huart4,UART_IT_RTO))
	{
        __HAL_UART_CLEAR_IT(&huart4,UART_CLEAR_RTOF);
                    
        /* Disable the DMA */
        __HAL_DMA_DISABLE(&hdma_uart4_rx);
                  
        /* Derive the receiving data length from the NDTR register */
        RxLen = USB2UART_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
                
        /* Clear DMA TCIF2& HTIF2 flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_uart4_rx,DMA_FLAG_TCIF2_6);
                  
        /* Reset the DMA NDTR */
        __HAL_DMA_SET_COUNTER(&hdma_uart4_rx,USB2UART_RXBUFFSIZE);
        UART4RxLen = RxLen;
        UART4RxEndFlag = 1; //标志已经成功接收到一包等待处理
//	    for(i = 0;i<RxLen;i++)
//	    {
//	        USB2UART_aTxBuffer0[i] = USB2UART_aRxBuffer0[i];
//	    }
//	    /* Progress the reveiving data */
//	    USB2UART_SendData(USB2UART_aTxBuffer0,RxLen);
                    
        /* Enable the DMA */
        __HAL_DMA_ENABLE(&hdma_uart4_rx);
				
	}
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
    u32 RxLen;
	volatile u32 a = 0;

//  HAL_UART_IRQHandler(&huart4);
    if(__HAL_UART_GET_IT(&huart5,UART_IT_RTO))
    {
        __HAL_UART_CLEAR_IT(&huart5,UART_CLEAR_RTOF);
				
        /* Disable the DMA */
        __HAL_DMA_DISABLE(&hdma_uart5_rx);
			  
        /* Derive the receiving data length from the NDTR register */
        RxLen = RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&hdma_uart5_rx);
			
        /* Clear DMA TCIF2& HTIF2 flag */
        __HAL_DMA_CLEAR_FLAG(&hdma_uart5_rx,DMA_FLAG_TCIF0_4);
			  
        /* Reset the DMA NDTR */
        __HAL_DMA_SET_COUNTER(&hdma_uart5_rx,RXBUFFSIZE);
        UART5RxLen = RxLen;
        UART5RxEndFlag = 1; //标志已经成功接收到一包等待处理
        /* Enable the DMA */
        __HAL_DMA_ENABLE(&hdma_uart5_rx);
    }
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
