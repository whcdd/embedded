#ifndef __CONFIG_H
#define __CONFIG_H


#include "stdint.h"
#include "main.h"
#include "clock_init.h"
#include "debug_uart5.h"
#include "led.h"
#include "key.h"
#include "DynamixelMotor.h"
#include <stdlib.h>
#include <string.h>
#include "usb2uart.h"
#include "MTi_630.h"
#include "sdram.h"
#include "malloc.h"
#include "nand.h"    
#include "ftl.h"
#include "myiic.h"
#include "MCU.h"
#include "iwdg.h"
#include "myiic.h"
#include "stdint.h"

/** 
  * @brief  HAL Status structures definition  
  */  
typedef enum 
{
  UART_DATA_ERROR       = 0,
	UART_SEND_COMPLETE		= 1
} UART_sendStatusTypeDef;

//extern u8 usb2uart_flag;


u8 UARTSendData(UART_HandleTypeDef *huart,u8 *pDataBuf, u16 len);

void Error_Handler(void);
void assert_failed(uint8_t *file, uint32_t line);
void Cache_Enalble(void);
void Sys_Init(void);
#endif 
