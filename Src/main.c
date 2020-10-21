/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      main.c                                                                                                      ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   ������                                                                                                      ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/09/25                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/
#include "main.h"
#include "debug_uart5.h"
#include "usb2uart.h"
#include "stm32f7xx_it.h"
#include "commandProcess.h"
#include "dataFrame.h"

/* SDRAM Memory */
// MAX ADDRESS IS 0XC1FF FFFF
#define SDRAMSIZE 0xffff
//u8 sdramMemory[SDRAMSIZE] __attribute__((at(0XC0000000)));
u8 *buf;
u8 *buf1;
u8 buf2[NAND_ECC_SECTOR_SIZE];

/*********************************************************************************************************************************/
/**Function Name:         main                                                                                                  **/
/**Function Description:  ������                                                                                                **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
int main(void)
{
// test nandflash
//	u8 *backbuf;	
//	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//����һ�������Ļ���
//	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//����һ�������Ļ���
//	FTL_ReadSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//Ԥ�ȶ�ȡ����0����������,��ֹ��д�����ļ�ϵͳ��.
    
    Sys_Init();
    while (1)
    {
        IWDG_Feed();    			//ι��
        Battery_Protect();
        Fan_Monitor();
        
        Test_Fuction();
//        Data_Receive();                                                                         
//        Data_Send();
    }
}



void Test_Fuction()
{
//  u8 test[] = "hello";
//  u8 test1[] = {0xff,0xfd,0x0C,0x00,0x93,0x05,0x02,0x11,0x05,0x12,0x11,0x22,0x33,0x44,0xaa,0xbb};
	u16 i;
	u8 key;
//	u8 *buf;
	
//test nandflash
    for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+3;	        //�������(�����,����t��ֵ��ȷ��) 
    FTL_WriteSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);         //д������
    key=FTL_ReadSectors(buf2,2,NAND_ECC_SECTOR_SIZE,1);     //��ȡ����
	  
//test motor uart	  
    if(UART1RxEndFlag)
    {
        DXL_SendData(DXL_aRxBuffer,UART1RxLen);
		UART1RxEndFlag = 0;
        UART1RxLen = 0;
    }
	  
//test usb uart	  
    if(UART4RxEndFlag)
	{	
        USB2UART_SendData(USB2UART_aRxBuffer0,UART4RxLen);

		UART4RxEndFlag = 0;
		UART4RxLen = 0;
	}

//test debug uart
    if(UART5RxEndFlag)
    {
        UART5_SendData(aRxBuffer,UART5RxLen);
        UART5RxEndFlag = 0;	 
        UART5RxLen = 0;
    }

//  HAL_Delay(1000);

}


