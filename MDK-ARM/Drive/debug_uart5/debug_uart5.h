#ifndef __DEBUG_UART5
#define __DEBUG_UART5

#include "main.h"
#include "config.h"

#define RXBUFFSIZE 200

extern unsigned char aRxBuffer[RXBUFFSIZE];
extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;

void Debug_UART5_Init(unsigned int baud);
void UART5_SendData(u8 *databuf, u16 len);


#endif
