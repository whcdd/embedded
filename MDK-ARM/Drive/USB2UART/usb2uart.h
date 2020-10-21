#ifndef  __USB2UART_H
#define  __USB2UART_H

#include "config.h"

/** @defgroup UART Buffer Size   
  * @{
  */
#define USB2UART_RXBUFFSIZE 255
#define USB2UART_RXBUFFSIZE1 10
/**
  * @}
  */
	
/** @defgroup DMA Double buffer
  * @{
  */
#define MEM0 0
#define MEM1 1
/**
  * @}
  */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart4;
//extern u8 USB2UART_aRxBuffer0[USB2UART_RXBUFFSIZE];
extern __align(4) u8 USB2UART_aRxBuffer0[USB2UART_RXBUFFSIZE] __attribute__((at(0XC0000700)));
////extern u8 USB2UART_aRxBuffer1[USB2UART_RXBUFFSIZE1];
//extern u8 USB2UART_aTxBuffer0[USB2UART_RXBUFFSIZE];
extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_uart4_rx;

extern u8 senddataflag;

void USB2UART_Init(u32 baud);

void USB2UART_SendData(u8 *databuf, u16 len);

#endif
