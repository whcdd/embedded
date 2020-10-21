#include "usb2uart.h"


/* External variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;
//u8 USB2UART_aRxBuffer0[USB2UART_RXBUFFSIZE] = {0};
__align(4) u8 USB2UART_aRxBuffer0[USB2UART_RXBUFFSIZE] __attribute__((at(0XC0000700)));
//u8 USB2UART_aRxBuffer1[USB2UART_RXBUFFSIZE1] = {0};
//u8 USB2UART_aTxBuffer0[USB2UART_RXBUFFSIZE] = {0};
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_uart4_rx;
u8 senddataflag;

static void USB2UART_DMA_Init(void);
static void USB2UART_ReceiverTimeoutInit(void);
void UART_Transmit(UART_HandleTypeDef huart,u8 *databuf, u8 len);

void USB2UART_Init(u32 baud)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = baud;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_8;
    huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart4) != HAL_OK)
    {
    Error_Handler();
    }

    /* Initialize USART RX DMA */
    USB2UART_DMA_Init();

    /* Enable USART receiving timeout mode */
    USB2UART_ReceiverTimeoutInit();

    /* Enable UART DMA transmitter&receiver   */
    SET_BIT(huart4.Instance->CR3, USART_CR3_DMAT|USART_CR3_DMAR);
}

/**
  * @brief USART4  Send Motor Data 
  * @param databuf: the starting address of the buffer memory    
  * @param len: the size of the buffer
  * @retval None
  */
void USB2UART_SendData(u8 *databuf, u16 len)
{
    UARTSendData(&huart4,databuf,len);
}

/**
  * @brief USART4 DMA Initialization Function
  * @param NONE
  * @retval None
  */
static void USB2UART_DMA_Init(void) 
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
        /* USART4 DMA Init */
    /* USART4_TX Init */
    // 使用单缓冲区
    hdma_uart4_tx.Instance = DMA1_Stream4;
    hdma_uart4_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_tx.Init.Mode = DMA_NORMAL;
    hdma_uart4_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_tx) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&huart4,hdmatx,hdma_uart4_tx);

    //		/* Set Peripheral and buffer address */
    //		HAL_DMA_Start(&hdma_uart4_tx,(u32)USB2UART_aTxBuffer0,(u32)&huart4.Instance->TDR,USB2UART_RXBUFFSIZE);

        /* USART2_RX Init */
    // 使用双缓冲区
    hdma_uart4_rx.Instance = DMA1_Stream2;
    hdma_uart4_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&huart4,hdmarx,hdma_uart4_rx);

    /* Set Peripheral and buffer address */
     HAL_DMA_Start(&hdma_uart4_rx,(u32)&huart4.Instance->RDR,(u32)USB2UART_aRxBuffer0,USB2UART_RXBUFFSIZE);
}

/**
  * @brief  Update on the fly the receiver timeout value in RTOR register.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *                    the configuration information for the specified UART module.
  * @param  TimeoutValue receiver timeout value in number of baud blocks. The timeout
  *                     value must be less or equal to 0x0FFFFFFFF.
  * @retval None
  */
static void USB2UART_ReceiverTimeoutInit(void)
{
    /* Set the timeout of 2 character times (2*10bit) */
    HAL_UART_ReceiverTimeout_Config(&huart4,20);
    HAL_UART_EnableReceiverTimeout(&huart4);

    /* Enable receive timeout interrupt */
    __HAL_UART_ENABLE_IT(&huart4,UART_IT_RTO);
}

