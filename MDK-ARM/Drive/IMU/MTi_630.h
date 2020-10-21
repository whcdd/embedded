#ifndef __MTI_630_H
#define __MTI_630_H

#include "config.h"


/** @defgroup IMU BUFFER SIZE
  * @{
  */
#define MTi_630_RXBUFFSIZE 130
/**
  * @}
  */
	
/** @defgroup checkSumComplement Status
  * @{
  */
#define VALID   1
#define INVALID 0
/**
  * @}
  */
	
/** @defgroup Buffer definition
* @{
*/
#define MTI630_BUFFER0   0
#define MTI630_BUFFER1   1
/**
  * @}
  */
	

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern __align(4) u8 MTi_630_aRxBuffer0[MTi_630_RXBUFFSIZE] __attribute__((at(0XC0000000)));
extern __align(4) u8 MTi_630_aRxBuffer1[MTi_630_RXBUFFSIZE] __attribute__((at(0XC0000100)));
extern u8  *MTi_630_aRxBuffer;


extern u8 MTi_630_flag;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;

/* Function declaration --------------------------------------------------------*/

/* Initialization and functions  ****************************/
void MTi_630_Init(u32 baud);

/* IMU operation functions *****************************************************/
void MTi_630_SendData(u8 *databuf, u8 len);

/* Other operation functions */
u8 copySrcBufferToDesMem(u8* SrcBuffer,u8* DesMem,u16 Len);


#endif
