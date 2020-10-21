/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      dataFrame.h                                                                                            ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   Êý¾Ý×éÖ¡                                                                                               ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/06/23                                                                                                    ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATAFRAME_H
#define __DATAFRAME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include "main.h"
#include "usb2uart.h"
#include "commandProcess.h"
	

/* Exported types ------------------------------------------------------------*/
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
#define SEND_BUFFSIZE 500
#define MOTOR_BUFFSIZE 400

/* Exported constants --------------------------------------------------------*/
//extern u8 *Motor_Buffer;

extern volatile u8 *Left_Buffer;
extern volatile u8 *Right_Buffer;
extern u8 *Motor_Mid_Buffer;
//extern volatile u8 *sendBuffer;

extern u8 IMU_Ready_Flag;
extern u8 MCU_Ready_Flag;
extern u8 DXL_Ready_Flag;
extern u8 Left_Ready_Flag;
extern u8 Right_Ready_Flag;

/* Exported functions prototypes ---------------------------------------------*/
void Data_Send(void);
void Packet_Frame(void);
void MCU_Frame(void);
void Left_Frame(void);
void Right_Frame(void);
void Motor_Frame(void);
void IMU_Frame(void);

#ifdef __cplusplus
}
#endif

#endif /* __DATAFRAME_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
