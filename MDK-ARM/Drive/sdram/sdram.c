#include "sdram.h"

/* External variables ---------------------------------------------------------*/
SDRAM_HandleTypeDef hsdram1;



/**
* @brief FMC initialization function
* @param None
* @retval None
*/

void SDRAM_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 8;
  SdramTiming.SelfRefreshTime = 6;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
	
	/* Send SDRAM Initialization Sequence */
	SDRAM_Initialization_Sequence(&hsdram1);
	
  /* Config auto refresh frequency 60ms */
	HAL_SDRAM_SetAutoRefreshNumber(&hsdram1,823);
}

/* SDRAM Initialization Sequence */
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
		u32 modeRegesiterValue=0;
		
		SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0);
		HAL_Delay(500); 
		SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);
		SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);
		/* Config the mode regester value */
	  modeRegesiterValue=(u32)SDRAM_MODEREG_BURST_LENGTH_1|
														SDRAM_MODEREG_ADDRESS_MODE_SEQUENTIAL|
														SDRAM_MODEREG_CAS_LATENCY_3|
														SDRAM_MODEREG_SINGLE_WRITE;
		SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,modeRegesiterValue);
}

/* SDRAM SEND CMD  */
static u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval)
{
		u32 target_bank=0;
		FMC_SDRAM_CommandTypeDef Command;
		
		if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;
		else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;
		
		
		Command.AutoRefreshNumber=refresh;
		Command.CommandMode=cmd;
		Command.CommandTarget=target_bank;
		Command.ModeRegisterDefinition=regval;
		
		if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000)==HAL_OK)
		{
				return 1;
		}
		else 
		{
				return 0;
		}
}

