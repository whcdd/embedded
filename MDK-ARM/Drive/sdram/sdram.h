#ifndef __SDRAM_H
#define __SDRAM_H

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "main.h"

typedef __IO u32 vu32;

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SDRAM_Initialization_Private_Functions
  * @{
  */
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);

/**
  * @}
  */
	
/** @addtogroup SDRAM_Send_Command_Private_Functions
  * @{
  */
static u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
/**
  * @}
  */

// SDRAM setting parameters
#define SDRAM_MODEREG_BURST_LENGTH_1					((u16)0X0000)
#define SDRAM_MODEREG_BURST_LENGTH_2					((u16)0X0001)
#define SDRAM_MODEREG_BURST_LENGTH_4					((u16)0X0002)
#define SDRAM_MODEREG_BURST_LENGTH_8					((u16)0X0003)
#define SDRAM_MODEREG_BURST_LENGTH_FULL_PAGE	((u16)0X0007)
#define SDRAM_MODEREG_ADDRESS_MODE_SEQUENTIAL ((u16)0X0000)
#define SDRAM_MODEREG_ADDRESS_MODE_INTERLEAVE ((u16)0X0008)
#define SDRAM_MODEREG_CAS_LATENCY_2						((u16)0X0020)
#define SDRAM_MODEREG_CAS_LATENCY_3						((u16)0X0030)
#define SDRAM_MODEREG_BURST_WRITE  						((u16)0X0000)
#define SDRAM_MODEREG_SINGLE_WRITE  					((u16)0X2000)


/* External variables ---------------------------------------------------------*/
extern SDRAM_HandleTypeDef hsdram1;

/* SDRAM Memory */
// max SDRAMSIZE 2^25 = 33554132 = 0x200 0000
#define Bank5_SDRAM_ADDR ((u32)(0XC0000000))


/* Function declaration --------------------------------------------------------*/

/* Initialization and functions  ****************************/
void SDRAM_FMC_Init(void);

/* SDRAM operation functions *****************************************************/



#endif
