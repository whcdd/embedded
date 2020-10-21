#include "MTi_630.h"

/* External variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;
__align(4) u8 MTi_630_aRxBuffer0[MTi_630_RXBUFFSIZE] __attribute__((at(0XC0000000)));
__align(4) u8 MTi_630_aRxBuffer1[MTi_630_RXBUFFSIZE] __attribute__((at(0XC0000100)));
u8  *MTi_630_aRxBuffer;
u8 MTi_630_flag=0;

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup UART_Private_Functions
  * @{
  */
static void MTi_630_DMA_Init(void);

void MTi_630_Init(u32 baud)
{
     GPIO_InitTypeDef GPIO_InitStruct = {0};
     
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pins : PB14 PB15 */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
              
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
     
    huart2.Instance = USART2;
    huart2.Init.BaudRate = baud;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Initialize USART RX DMA */
    MTi_630_DMA_Init();
    
    /* Enable UART DMA transmitter&receiver   */
    SET_BIT(huart2.Instance->CR3, USART_CR3_DMAT|USART_CR3_DMAR);
    
    /* Wait at least 500ms for entering measure mode */
    HAL_Delay(500);
    }

    /**
    * @brief USART2 DMA Initialization Function
    * @param NONE
    * @retval None
    */
    static void MTi_630_DMA_Init(void) 
    {

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
            /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
     
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart2,hdmatx,hdma_usart2_tx);
        
        /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&huart2,hdmatx,hdma_usart2_rx);
        
    /* Set Peripheral and buffer address */
//  HAL_DMA_Start_IT(&hdma_usart2_rx,(u32)&huart2.Instance->RDR,(u32)MTi_630_aRxBuffer0,MTi_630_RXBUFFSIZE);
    HAL_DMAEx_MultiBufferStart_IT(&hdma_usart2_rx, (u32)&huart2.Instance->RDR, (u32)MTi_630_aRxBuffer0, (u32)MTi_630_aRxBuffer1, MTi_630_RXBUFFSIZE);
        
    /* DMA interrupt init */
    /* DMA1_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

/**
  * @brief IMU  Send Data to IMU 
  * @param databuf: the starting address of the buffer memory    
  * @param len: the size of the buffer
  * @retval None
  */
void MTi_630_SendData(u8 *databuf, u8 len)
{
    UARTSendData(&huart2,databuf,len);
//	while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC) != SET);
}

/**
  * @brief IMU  Copy SrcBuffer to DesMem
  * @param SrcBuffer: Souce Buffer
	* @param DesMem: Destination Memory
  * @param Len: the size of the buffer
  * @retval None
  */

u8 copySrcBufferToDesMem(u8* SrcBuffer,u8* DesMem,u16 Len)
{	
    memcpy(DesMem,SrcBuffer,Len*sizeof(u8));
    return 1;
}
