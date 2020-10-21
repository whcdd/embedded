#include "config.h"

//u8 usb2uart_flag=0;

/*********************************************************************************************************************************/
/**Function Name:         Sys_Init                                                                                                  **/
/**Function Description:  系统初始化函数                                                                                                **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/

void Sys_Init()
{
    Cache_Enalble();
    HAL_Init();
	SystemClock_Config();
	KEY_Init();
    LED_Init();
    delay_init(216);                     //延时初始化
	SDRAM_FMC_Init();
    //所有串口初始化
    MTi_630_Init(921600);
	USB2UART_Init(6000000);
	DXLMotor_Init(3000000);
    Debug_UART5_Init(921600);
    
	IWDG_Init(IWDG_PRESCALER_64,500); 	 //分频数为64,重载值为500,溢出时间为1s	
	IIC_Init();
	Fan_Init();	
//	Emerge_Stop_Init();
//	Bat_Protect_Init();
//	FTL_Init();
//	RF_Init();
    
}


/*UART中断程序，已不使用 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	  u8 test[] = "OK\r\n";
//	  /* Debug uart5 */
//	  if(huart->Instance == UART5)
//		{
//				unsigned char ch[2] = {'O','K'};
//				HAL_UART_Transmit(huart,ch,2,1000);
//				while(__HAL_UART_GET_FLAG(&huart5,UART_FLAG_TC) != SET);
//				
//				HAL_UART_Receive_IT(&huart5,aRxBuffer,RXBUFFSIZE);
//	  }
//		/* usart1 DXL */
//		if(huart->Instance == USART1)
//		{
//			  
//			  /* Pull the data out of the BUFFER */
//			  	  /* Transmit the data */
//				DXL_SendData(test,4);
//			  DXL_SendData(DXL_aRxBuffer,DXL_RXBUFFSIZE);
//			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14|GPIO_PIN_15);
////			  DXL_ReceiveData();
//			
//				HAL_UART_Receive_IT(&huart1,DXL_aRxBuffer,DXL_RXBUFFSIZE);
//		}
//		/*  USB2UART  */
//		if(huart->Instance == UART4)
//		{
//				usb2uart_flag = 1;
////		    USB2UART_SendData(test,4);
////			  USB2UART_SendData("\r\n",2);
////			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14|GPIO_PIN_15);			
////			  HAL_UART_Receive_IT(&huart4,USB2UART_aRxBuffer,USB2UART_RXBUFFSIZE);
//		}
//		/*  MTi_630 USART2  */
//		if(huart->Instance == USART2)
//		{
//				MTi_630_SendData(test,4);
//			
//			
//		    HAL_UART_Receive_IT(&huart2,MTi_630_aRxBuffer0,MTi_630_RXBUFFSIZE);
//		}
}

/**
  * @brief UART Send Data 
  * @param databuf: the starting address of the buffer memory    
  * @param len: the size of the buffer
  * @retval UART_DATA_ERROR=0;UART_SEND_COMPLETE=1
  */
u8 UARTSendData(UART_HandleTypeDef *huart,u8 *pDataBuf, u16 len)
{
    if((pDataBuf == NULL) || len == 0)
    {
        return UART_DATA_ERROR;
    }
    
    huart->TxXferSize = len;
    huart->TxXferCount = len;
    
    while(huart->TxXferCount > 0)
    {
        huart->Instance->TDR = *pDataBuf;
        while(__HAL_UART_GET_FLAG(huart,UART_FLAG_TXE) != SET);

        pDataBuf++;
        huart->TxXferCount--;	
    }
    return UART_SEND_COMPLETE;
}



void Cache_Enalble(void)
{
    SCB_EnableICache();//Enable I-Cache
    SCB_EnableDCache();//Enable D-Cache
	SCB->CACR|=1<<2;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
