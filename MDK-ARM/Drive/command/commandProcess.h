/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      commandProcess.h                                                                                            ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   数据分发与组帧                                                                                               ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/06/22                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMANDPROCESS_H
#define __COMMANDPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include "main.h"
#include "usb2uart.h"
	

/* Exported types ------------------------------------------------------------*/
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;

/* Exported constants --------------------------------------------------------*/
extern u8 MCU_Read_Flag;
extern u8 IMU_Read_Flag;
extern u8 Left_Read_Flag;
extern u8 Right_Read_Flag;
extern u8 Motor_Read_Flag;
extern u8 Motor_Read_Cmd;
extern u16 Sync_Read_Len;
extern u16 Bulk_Read_Len;
//extern u8 Motor_Length_Sign;

/* Exported functions prototypes ---------------------------------------------*/
void Data_Receive(void);
void Cmd_Distribute(u8 *InputBuffer);
void Config_Send(u8 *InputBuffer,u8 cmdLength);
void Req_Send(u8 *InputBuffer,u8 cmdLength);
void Cmd_Choose(u8 cmdNum,u8 *cmdWord,u8 cmdLength);
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size);
void Device_Choose(u8 devNum,u8 *devCmd,u8 devCmdLen);
void MCU_cmd_Send(u8 *cmdWord,u8 cmdLength);
void IMU_cmd_Send(u8 *cmdWord,u8 cmdLength);
void Left_cmd_Send(u8 *cmdWord,u8 cmdLength);
void Right_cmd_Send(u8 *cmdWord,u8 cmdLength);
void Motor_cmd_Send(u8 *cmdWord,u8 cmdLength);
void Motor_Data_Read(u8 *cmdWord,u8 currentPoint);
void Read_Dev_Choose(u8 devNum,u8 Cmd_L,u8 Cmd_H);

#ifdef __cplusplus
}
#endif

#endif /* __COMMANDPROCESS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
