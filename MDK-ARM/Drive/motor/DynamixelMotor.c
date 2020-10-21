#include "DynamixelMotor.h"
#include <string.h>

static void DXL_ReceiverTimeoutInit(void);

static void DXL_DMA_Init(void);
void DXL_UART_Transmit(u8 *databuf, u8 len);

/* External variables --------------------------------------------------------*/
__align(4) u8 DXL_aRxBuffer[DXL_RXBUFFSIZE] __attribute__((at(0XC0000500)));
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_uart1_tx;
DMA_HandleTypeDef hdma_uart1_rx;

void DXL_DIR(DXL_DIRTypeDef dir)  
{
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,(GPIO_PinState) dir);
} 

DXL_StatusTypeDef DXLMotor_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* DXL_DIR Initialization*/
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    /* By default data receiving mode */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

    /*Configure GPIO pin : PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* USART1 Initialization */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = baud;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_8;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
        return (DXL_StatusTypeDef)DXL_ERROR;
    }
    
    DXL_DMA_Init();

  /* Enable USART receiving timeout mode */
    DXL_ReceiverTimeoutInit();

    /* Enable UART DMA transmitter&receiver   */
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT|USART_CR3_DMAR);
    
    return (DXL_StatusTypeDef)DXL_OK;
}

static void DXL_DMA_Init(void) 
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();
    /* USART1 DMA Init */
    /* USART1_TX Init */
    // 使用单缓冲区
    hdma_uart1_tx.Instance = DMA2_Stream7;
    hdma_uart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart1_tx.Init.Mode = DMA_NORMAL;
    hdma_uart1_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_uart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart1_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1,hdmatx,hdma_uart1_tx);

    hdma_uart1_rx.Instance = DMA2_Stream2;
    hdma_uart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart1_rx.Init.Mode = DMA_NORMAL;
    hdma_uart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_uart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart1_rx) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&huart1,hdmarx,hdma_uart1_rx);	
		
	/* Set Peripheral and buffer address */
	HAL_DMA_Start(&hdma_uart1_rx,(u32)&huart1.Instance->RDR,(u32)DXL_aRxBuffer,DXL_RXBUFFSIZE);
}

/**
  * @brief USART1 RS485  Send Motor Data 
  * @param databuf: the starting address of the buffer memory    
  * @param len: the size of the buffer
  * @retval None
  */
DXL_StatusTypeDef DXL_SendData(u8 *databuf, u8 len)
{
    DXL_DIR(DE);
	/* Transmit the data */
	UARTSendData(&huart1,databuf,len);
	while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC) != SET);

	DXL_DIR(RE);
	return (DXL_StatusTypeDef)DXL_OK;
}


/**
  * @brief  Pull the data out of the buffe and put it into the memory user creates 
  * @param databuf: the starting address of the buffer memory    
  * @param len: the size of the buffer
  * @retval None
  */
DXL_StatusTypeDef DXL_ReceiveData(u8 *datades,u8 len)
{
	  
	  return (DXL_StatusTypeDef)DXL_OK;
}

/**
  * @brief Send complete instruction to motor according Protocol
  * @param DXL_Handler: DXL handler 
  * @retval DXL status
  */
//DXL_StatusTypeDef DXL_ProtocolSendData(DXL_HandlerTypeDef *DXL_Handler)
//{
//    u16 i=0;
//    u8 CRC_L=0,CRC_H=0;
//    u16 CRC16=0;
//    /*  Define A DataPacket used for sending a complete data to cal  */
//    u8 *DataPacket;
//    u8 PacketLen = 7 + DXL_Handler->len;

//    u8 id = DXL_Handler->id;
//    u16 len = DXL_Handler->len;
//    /*  distribute the memory to DataPacet */
//    DataPacket = (u8 *)malloc(PacketLen*sizeof(u8));
//    memset(DataPacket,0,PacketLen);

//    /* Generate a complete data packet */
//    DataPacket[0] = 0xFF;
//    DataPacket[1] = 0xFF;
//    DataPacket[2] = 0xFD;
//    DataPacket[3] = 0x00;
//    DataPacket[4] = id;
//    DataPacket[5] = (DXL_Handler->len)&0xff;
//    DataPacket[6] = (DXL_Handler->len)>>8;
//    DataPacket[7] = DXL_Handler->Ins;

//    if(len > 3)
//    {
//        for(i=8;i<len+5;i++)
//        {
//            DataPacket[i] = DXL_Handler->Param[i-8];
//        }
//    }	  

//    /*  Calculate the CRC  */
//    //	  CRC16 = update_crc(0, DataPacket, 5+DXL_Handler->len);
//    CRC_L = CRC16 & 0xff;
//    CRC_H = CRC16 >> 8;

//    DataPacket[len+5] =  CRC_L;
//    DataPacket[len+6] =  CRC_H;

//    /*  Send the data  */
//    DXL_SendData(DataPacket,7+len);

//    /*  Free the  memory */
//    free(DataPacket);
//    return DXL_OK;
//}

//DXL_StatusTypeDef DXL_ProtocolReceiveData(DXL_HandlerTypeDef *DXL_Handler)
//{
//    return DXL_OK;
//}

static void DXL_ReceiverTimeoutInit(void)
{
	/* Set the timeout of 2 character times (2*10bit) */
	HAL_UART_ReceiverTimeout_Config(&huart1,20);
    HAL_UART_EnableReceiverTimeout(&huart1);
	
    /* Enable receive timeout interrupt */
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RTO);
}
