#include "debug_uart5.h"

unsigned char aRxBuffer[RXBUFFSIZE] = {0};

UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_uart5_tx;
DMA_HandleTypeDef hdma_uart5_rx;

static void UART5_DMA_Init(void);
static void UART5_ReceiverTimeoutInit(void);
void UART5_SendData(u8 *databuf, u16 len);

/**
  * @brief UART5 Initialization Function
  * @param baud
  * @retval None
  */
void Debug_UART5_Init(unsigned int baud)
{
    huart5.Instance = UART5;
    huart5.Init.BaudRate = baud;
    huart5.Init.WordLength = UART_WORDLENGTH_8B;
    huart5.Init.StopBits = UART_STOPBITS_1;
    huart5.Init.Parity = UART_PARITY_NONE;
    huart5.Init.Mode = UART_MODE_TX_RX;
    huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart5.Init.OverSampling = UART_OVERSAMPLING_16;
    huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
        Error_Handler();
    }
    /* Initialize USART RX DMA */
	UART5_DMA_Init();
	
	/* Enable USART receiving timeout mode */
	UART5_ReceiverTimeoutInit();
	
	/* Enable UART DMA transmitter&receiver   */
	SET_BIT(huart5.Instance->CR3, USART_CR3_DMAT|USART_CR3_DMAR);

}

static void UART5_DMA_Init(void) 
{

    /* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();
	/* USART4 DMA Init */
    /* USART4_TX Init */
	// 使用单缓冲区
    hdma_uart5_tx.Instance = DMA1_Stream7;
    hdma_uart5_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart5_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart5_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart5_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart5_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart5_tx.Init.Mode = DMA_NORMAL;
    hdma_uart5_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_uart5_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart5,hdmatx,hdma_uart5_tx);
		
    /* Set Peripheral and buffer address */
//	HAL_DMA_Start(&hdma_uart4_tx,(u32)USB2UART_aTxBuffer0,(u32)&huart4.Instance->TDR,USB2UART_RXBUFFSIZE);
		
	/* USART2_RX Init */
	// 使用双缓冲区
    hdma_uart5_rx.Instance = DMA1_Stream0;
    hdma_uart5_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart5_rx.Init.Mode = DMA_NORMAL;
    hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart5,hdmarx,hdma_uart5_rx);
		
	/* Set Peripheral and buffer address */
	HAL_DMA_Start(&hdma_uart5_rx,(u32)&huart5.Instance->RDR,(u32)aRxBuffer,RXBUFFSIZE);
}

/**
  * @brief  Update on the fly the receiver timeout value in RTOR register.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART module.
  * @param  TimeoutValue receiver timeout value in number of baud blocks. The timeout
  *                     value must be less or equal to 0x0FFFFFFFF.
  * @retval None
  */
static void UART5_ReceiverTimeoutInit(void)
{
	/* Set the timeout of 2 character times (2*10bit) */
	HAL_UART_ReceiverTimeout_Config(&huart5,20);
    HAL_UART_EnableReceiverTimeout(&huart5);
	
	/* Enable receive timeout interrupt */
	__HAL_UART_ENABLE_IT(&huart5,UART_IT_RTO);
}

void UART5_SendData(u8 *databuf, u16 len)
{
	/* Transmit the data */
	UARTSendData(&huart5,databuf,len);
}

