#include "MCU.h"
/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      MCU.c                                                                                                 ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   MCU板上状态监测                                                                                              ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/07/03                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/

float temprH = 36.0,temprL = 33.0;
u8 Emerge_flag = 0;
u8 Under_Voltage_flag = 0;
u8 Over_Voltage_flag = 0;


void Bat_Protect_Init(void);
void Battery_Protect(void);
void Emerge_Stop_Init(void);
void Emerge_Stop_Monitor(void);
void Fan_Init(void);
void Fan_Monitor(void);

/*********************************************************************************************************************************/
/**Function Name:         Fan_Init                                                                                      **/
/**Function Description:  电池保护系统初始化                                                                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Fan_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOA时钟
	
	//初始化风扇输入，初始化为低电平。高电平时蜂鸣器报警
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);//初始化蜂鸣器默认电平为低电平
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);//初始化蜂鸣器默认电平为低电平
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);//初始化蜂鸣器默认电平为高电平
}



/*********************************************************************************************************************************/
/**Function Name:         Bat_Protect_Init                                                                                      **/
/**Function Description:  电池保护系统初始化                                                                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Bat_Protect_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟

	//初始化OUTA，OUTB
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//初始化蜂鸣器输入，初始化为低电平。高电平时蜂鸣器报警
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//初始化蜂鸣器默认电平为低电平
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);//初始化蜂鸣器默认电平为高电平
}


/*********************************************************************************************************************************/
/**Function Name:         Battery_Protect                                                                                       **/
/**Function Description:  电池保护监测，当系统欠压或者过压时，通过将蜂鸣器输入置高来使蜂鸣器响起                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/

void Battery_Protect(void)
{
	GPIO_PinState outA,outB;
//	u8 test[] = "hello";
//	u8 test1[] = "hallo";
//	u8 test3[] = "hillo";
//	u8 test4[] = "hullo";
//	u8 test5[] = "hMllo";
	outA = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);
	outB = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
	
//	if(outA == GPIO_PIN_RESET)
//	{USB2UART_SendData(test,6);}else{USB2UART_SendData(test1,6);}
//	if(outB == GPIO_PIN_RESET)
//	{USB2UART_SendData(test3,6);}else{USB2UART_SendData(test4,6);}
//	
//	if((outA == GPIO_PIN_RESET)||(outB == GPIO_PIN_RESET))
    if(outA == GPIO_PIN_RESET)
	{
		Under_Voltage_flag = 1;
		Over_Voltage_flag = 1;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
//		USB2UART_SendData(test5,6);
	}else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	}
}


/*********************************************************************************************************************************/
/**Function Name:         Battery_Protect                                                                                       **/
/**Function Description:  电池保护监测，当系统欠压或者过压时，通过将蜂鸣器输入置高来使蜂鸣器响起                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/

void Emerge_Stop_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOA时钟

	//初始化OUTA，OUTB
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;   //上升沿触发
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;			//下拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//中断线12
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,0);   //抢占优先级为0，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线12  
}

/*********************************************************************************************************************************/
/**Function Name:         Battery_Protect                                                                                       **/
/**Function Description:  电池保护监测，当系统欠压或者过压时，通过将蜂鸣器输入置高来使蜂鸣器响起                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/

void Emerge_Stop_Monitor(void)
{
	GPIO_PinState emergency;
	emergency = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12);
	
	if(emergency == GPIO_PIN_RESET)
	{
		return;
	}else{
		Emerge_flag = 1;
		__HAL_UART_DISABLE(&huart1);
		__HAL_UART_DISABLE(&huart2);
	}
}

//for test
//void Emerge_Stop_Monitor(void)
//{
//	GPIO_PinState emergency;
//	emergency = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);
//	
//	if(emergency == GPIO_PIN_SET)
//	{
//		return;
//	}else{
//		Emerge_flag = 1;
//		__HAL_UART_DISABLE(&huart4);
//		__HAL_UART_DISABLE(&huart2);
//	}
//}


/*********************************************************************************************************************************/
/**Function Name:         Fan_Monitor                                                                                       **/
/**Function Description:  电池保护监测，当系统欠压或者过压时，通过将蜂鸣器输入置高来使蜂鸣器响起                                     **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/

void Fan_Monitor(void)
{
	
	float temput = I2C_LM75_Read();
	if(temput>temprH)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);//开启风扇
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);//开启风扇
	
	}else{
	
		if(temput<temprL)
		{
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);//关闭风扇
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);//关闭风扇
		}
	}
}
