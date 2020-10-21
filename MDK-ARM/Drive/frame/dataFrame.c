/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      dataFrame.c                                                                                                 ***/
/***Author:        caodandan                                                                                                   ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   ������֡                                                                                                    ***/
/**                                                                                                                            ***/
/**                                                                                                                            ***/
/***Others:                                                                                                                    ***/
/***Function List:                                                                                                             ***/
/**    1.                                                                                                                      ***/
/**    2.                                                                                                                      ***/
/***History:                                                                                                                   ***/
/**    1.Date:   2020/06/24                                                                                                    ***/
/**      Author: Caodandan                                                                                                     ***/
/**      Modification:                                                                                                         ***/
/**    2.                                                                                                                      ***/
/*********************************************************************************************************************************/
#include "dataFrame.h"
#include "main.h"
#include "MCU.h"


/* External variables -----------------------------------------------------------------------------------------------------------*/
__align(4) u8  Send_Buffer[SEND_BUFFSIZE] __attribute__((at(0XC0000300)));
__align(4) u8  Motor_Buffer[MOTOR_BUFFSIZE] __attribute__((at(0XC0000900)));
//u8  Send_Buffer[SEND_BUFFSIZE];
//u8 Motor_Buffer[100]; //�˴����ȴ�����
//u8 *Motor_Mid_Buffer = NULL;
u8 *IMU_Buffer;
u8 *MCU_Buffer;
volatile u8 *Left_Buffer = NULL;
volatile u8 *Right_Buffer = NULL;
//���豸���ݴ�����ɱ�־
u8 IMU_Ready_Flag = 0;
u8 MCU_Ready_Flag = 0;
u8 DXL_Ready_Flag = 0;
u8 Left_Ready_Flag = 0;
u8 Right_Ready_Flag = 0;
u16 Motor_Length_Sign = 4; //������֡��3���ֽڰ�ͷ����
u8 IMU_Start_Flag = 4;
u8 DXL_NReady_Flag = 0;
u16 currentLen = 0;
u8 Motor_Error_Flag = 0;
u8 IMU_Error_Flag = 0;

//volatile u8 *Send_Buffer;


/* Private function prototypes --------------------------------------------------------------------------------------------------*/
/** @addtogroup dataFrame_Private_Functions
  * @{
  */
void Data_Send(void);
void Packet_Frame(void);
void MCU_Frame(void);
void Left_Frame(void);
void Right_Frame(void);
void IMU_Frame(void);
void Motor_Frame(void);



/*********************************************************************************************************************************/
/**Function Name:         Data_Send                                                                                            **/
/**Function Description:  ��������                                                                                               **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Data_Send(void)
{
	if((MCU_Read_Flag) || (IMU_Read_Flag) || (Left_Read_Flag) || (Right_Read_Flag) || (Motor_Read_Flag))
	{
		Motor_Frame();
	 	IMU_Frame();
//		MCU_Frame();
	//	Left_Frame();
	//	Right_Frame();
		Packet_Frame();
	}
}
/*********************************************************************************************************************************/
/**Function Name:         MCU_Frame                                                                                             **/
/**Function Description:  MCU����������֡����                                                                                    **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void MCU_Frame(void)
{
	if((Motor_Read_Flag) && (DXL_Ready_Flag == 0))//�����δ�������ʱ
	{
		return;
	}
	
	if(MCU_Read_Flag)
	{
		MCU_Buffer[0] = 0x01;
		MCU_Buffer[1] = 0x00;
		MCU_Buffer[2] = 0x00;
		if(Emerge_flag)
		{
			MCU_Buffer[1] += 0x01 ;
		}
		if(Under_Voltage_flag)
		{
			MCU_Buffer[1] += 0x02 ;
		}
		if(Over_Voltage_flag)
		{
			MCU_Buffer[1] += 0x04 ;
		}
	}
	MCU_Ready_Flag = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Left_Frame                                                                                            **/
/**Function Description:  ѹ����������ŷ���������֡����                                                                          **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Left_Frame(void)
{
	Left_Ready_Flag  = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Motor_Frame                                                                                           **/
/**Function Description:  ѹ���������ҽŷ���������֡����                                                                          **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Right_Frame(void)
{
	Right_Ready_Flag = 1;
}
/*********************************************************************************************************************************/
/**Function Name:         Motor_Frame                                                                                           **/
/**Function Description:  �������������֡����                                                                                   **/
/**Inputs:                void                                                                                                  **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Motor_Frame(void)
{
	u16 i = 0;
//	u8 currentLen = 0;
	u8 test[100] = {0xff,0xfa,0x79,0xAB,0x55};
	u8 CRC_L;
	u8 CRC_H;
	u16 CRC16;
	Motor_Buffer[0] = 0x05;
	Motor_Buffer[1] = 0x00;
	Motor_Buffer[2] = 0x00;
	Motor_Buffer[3] = 0x00;
	
	
	CRC16 = update_crc(0, test, 8);
	CRC_L = CRC16 & 0xff;
	CRC_H = CRC16 >> 8;
	test[8] = CRC_L;
	test[9] = CRC_H;
	
	if(UART1RxEndFlag && Motor_Read_Flag)//���յ���������������ڴ�����ǰ��λ���Ѿ����Ͷ����ָ��
	{
//		if((DXL_aRxBuffer[0] !=0xff)||(DXL_aRxBuffer[1] !=0xff)|| (DXL_aRxBuffer[2] !=0xfd))
//		{
//			for(i = 0;i< UART1RxLen+0;i++)
//			{
//				test[i+5] = DXL_aRxBuffer[i];
//			}
//			USB2UART_SendData(test,UART1RxLen+5);
//			return;	
//		}
	
		switch(Motor_Read_Cmd)
		{
			case 0x02://read
				Motor_Length_Sign += UART1RxLen;
				for(i = 4;i<Motor_Length_Sign;i++)
				{
					Motor_Buffer[i] = DXL_aRxBuffer[i-4]; 
				}
				Motor_Buffer[3] = Motor_Length_Sign - 4;
//	 			DXL_SendData(DXL_aRxBuffer,UART1RxLen);
//				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);
				DXL_Ready_Flag = 1;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
				return;
			case 0x82://Sync Read
//				Sync_Read_Len--;
//				DXL_SendData(&Sync_Read_Len,1);
				for(i = 0;i<UART1RxLen;i++)
				{
					Motor_Buffer[i+Motor_Length_Sign] = DXL_aRxBuffer[i];
				}
				memset(DXL_aRxBuffer,0,UART1RxLen);
				Motor_Length_Sign += UART1RxLen;
				currentLen = Motor_Length_Sign - 4;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
			//ƴ������
				if(currentLen < Sync_Read_Len)
				{
//					DXL_SendData(test,6);
					DXL_NReady_Flag = 1;
				}
				if(currentLen == Sync_Read_Len)
				{
					Motor_Buffer[3] = currentLen;
//					DXL_SendData(&currentLen,1);
					currentLen = 0;
		//				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
					DXL_NReady_Flag = 0;
					Sync_Read_Len = 0;
					DXL_Ready_Flag = 1;
					return;
				}
				if(currentLen > Sync_Read_Len)
				{
//					DXL_SendData(test,6);
					Motor_Error_Flag = 1;
					Motor_Buffer[3] = currentLen;
//					DXL_SendData(&currentLen,1);
					currentLen = 0;
		//				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
					DXL_NReady_Flag = 0;
					Sync_Read_Len = 0;
					DXL_Ready_Flag = 1;
					return;
				}
				break;
			case 0x92://Bulk Read
//				Bulk_Read_Len--;
				for(i = 0;i<UART1RxLen;i++)
				{
					Motor_Buffer[i+Motor_Length_Sign] = DXL_aRxBuffer[i];
				}
//				memset(DXL_aRxBuffer,0,UART1RxLen);
				Motor_Length_Sign += UART1RxLen;
				currentLen = Motor_Length_Sign - 4;
				UART1RxEndFlag = 0;
				UART1RxLen = 0;
				if(currentLen < Bulk_Read_Len)
				{
//					DXL_SendData(test,6);
					DXL_NReady_Flag = 1;
				}
			//ƴ������,�����ݴ���һ������У�������ƴ����ɴ���
				if(currentLen == Bulk_Read_Len)
		//			if(Motor_Buffer[3] == 25)	
				{
		//				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
					Motor_Buffer[3] = currentLen;
//					for(j = 4;j < Bulk_Read_Len;j++)
//					{
//					
//					}
					currentLen = 0;
					DXL_NReady_Flag = 0;
					Bulk_Read_Len = 0;
					DXL_Ready_Flag = 1;
					return;
				}
				if(currentLen > Bulk_Read_Len)
				{
//					DXL_SendData(test,6);
					Motor_Error_Flag = 1;
					Motor_Buffer[3] = currentLen;
//					DXL_SendData(&currentLen,1);
					currentLen = 0;
		//				USB2UART_SendData(Motor_Buffer,Motor_Length_Sign);//�˴��������Ż�����IMUƴ����֡
					DXL_NReady_Flag = 0;
					Bulk_Read_Len = 0;
					DXL_Ready_Flag = 1;
					return;
				}
				break;
			default:
				//����λ�������豸���������
				break;
		}
	}
}

/*********************************************************************************************************************************/
/**Function Name:         IMU_Frame                                                                                             **/
/**Function Description:  �������������֡����                                                                                   **/
/**Inputs:                UINT8 blockNum                                                                                        **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void IMU_Frame(void)
{
	u8 i;
//	u8 test[] = "hello";
//	u16 checksum;

	if((Motor_Read_Flag) && (DXL_Ready_Flag == 0))//�����δ�������ʱ
	{
		return;
	}
	if(IMU_Read_Flag)
	{
//		IMU_Buffer = (u8 *)malloc(69*sizeof(u8));
//		memset(IMU_Buffer,0,69);
		IMU_Buffer[0] = 0x02;
		IMU_Buffer[1] = 0x00;
		IMU_Buffer[2] = 0x00;//ָ��λ��Ԥ��
		IMU_Buffer[3] = 0x41;
		
//		DXL_SendData(MTi_630_aRxBuffer,130);
		
		for(i = 0;i<130;i++)
		{
			if((MTi_630_aRxBuffer[i] == 0xFA) && (MTi_630_aRxBuffer[i+1] == 0xFF) && (MTi_630_aRxBuffer[i+2] == 0x36) && (MTi_630_aRxBuffer[i+3] == 0x3C) && (MTi_630_aRxBuffer[i+4] == 0x20) && (MTi_630_aRxBuffer[i+5] == 0x30))
			{
				IMU_Start_Flag = i;
				break;
			}
		}
		for(i = 4;i<69;i++)
		{
			IMU_Buffer[i] = MTi_630_aRxBuffer[IMU_Start_Flag];
			IMU_Start_Flag++;
//			IMU_Buffer[i] = 0xAC;
		}
		
		
//		for(i = 5;i<69;i++) //����preamble
//		{
//			checksum+=IMU_Buffer[i];
//		}
//		checksum = checksum & 0xff;
//		if(checksum != 0)
//		{
//			IMU_Error_Flag = 1;
//		}
		
		IMU_Ready_Flag = 1;
//		DXL_SendData(IMU_Buffer,69);
//		if(IMU_Read_Flag)
//		{
////			USB2UART_SendData(IMU_Buffer,69);
//			free(IMU_Buffer);
////			HAL_Delay(1000);
//			IMU_Read_Flag = 0;
//		}
		//�ӿں���ȡIMU����ֵ
	}
}


/*********************************************************************************************************************************/
/**Function Name:         Packet_Frame                                                                                           **/
/**Function Description:  ��������������֡����                                                                                    **/
/**Inputs:                UINT8 blockNum                                                                                        **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Packet_Frame(void)
{
	u16 Send_BufferLen;
	u16 i = 6,j = 0;
	u16 CRC16=0;
	u8 CRC_L=0,CRC_H=0;
//	u8 test1[] = {0x12,0x34,0x56};
//	u8 test2[] = {0xab,0xcd,0xef};
	
//	Send_Buffer = (u8 *)malloc(30*sizeof(u8));
//	memset(Send_Buffer,0,30);
	
	if((Motor_Read_Flag) &&(DXL_NReady_Flag) && (DXL_Ready_Flag == 0))//�����δ�������ʱ
	{
//		DXL_SendData(test1,3);
		return;
	}
	if((Motor_Read_Flag) &&(DXL_NReady_Flag) && (DXL_Ready_Flag == 0))//�����δ�������ʱ
	{
//		DXL_SendData(test1,3);
		return;
	}
	if((MCU_Ready_Flag)||(Left_Ready_Flag)||(Right_Ready_Flag)||(IMU_Ready_Flag)||(DXL_Ready_Flag))
	{
//		DXL_SendData(test2,3);
		Send_Buffer[0] = 0xFF;
		Send_Buffer[1] = 0xFA;
		Send_Buffer[2] = 0x00;
		Send_Buffer[3] = 0x00;
		Send_Buffer[4] = 0x55;
		Send_Buffer[5] = 0x00;
		if((MCU_Read_Flag) && (MCU_Ready_Flag))
		{
			for(j = 0;j < 3;j++)
			{
				Send_Buffer[i] = MCU_Buffer[j];
				i++;
			}
//			memset(MCU_Buffer,0,3);
			MCU_Read_Flag = 0;
			MCU_Ready_Flag = 0;
			
		}
	//	if(MCU_Read_Flag)
	//	{
	//		for(j = 0;j < 4;j++)
	//		{
	//			Send_Buffer[i] = MCU_Buffer[j];
	//			i++;
	//		}
	//		memset(MCU_Buffer,0,4);
	//		MCU_Read_Flag = 0;
//		MCU_Ready_Flag = 0;
	//	
	//	}
	//	if(Left_Read_Flag)
	//	{
	//		for(j = 0;j < 5;j++)
	//		{
	//			Send_Buffer[i] = Left_Buffer[j];
	//			i++;
	//		}
	//		memset(Left_Buffer,0,4);
	//		Left_Read_Flag = 0;
//	Left_Ready_Flag = 0;
	//	}
	//	if(Right_Read_Flag)
	//	{
	//		for(j = 0;j < 5;j++)
	//		{
	//			Send_Buffer[i] = Right_Buffer[j];
	//			i++;
	//		}
	//		memset(Right_Buffer,0,4);
	//		Right_Read_Flag = 0;
//	Right_Ready_Flag = 0;
	//	}
//	USB2UART_SendData(IMU_Buffer,69);
		if((IMU_Read_Flag) && (IMU_Ready_Flag))
		{
//			USB2UART_SendData(IMU_Buffer,69);
			if(IMU_Error_Flag)
			{
				Send_Buffer[5] += 0x16;
				IMU_Error_Flag = 0;
			}else
			{
				for(j = 0;j < 69;j++)
				{
					Send_Buffer[i] = IMU_Buffer[j];
					i++;
				}
			}
			
//			DXL_SendData(Send_Buffer,i);
//			free(IMU_Buffer);
//			memset(IMU_Buffer,0,MTi_630_RXBUFFSIZE);
			IMU_Read_Flag = 0;
			IMU_Ready_Flag = 0;
		}
		if(Motor_Read_Flag)
		{
			if(!DXL_Ready_Flag)
			{
				return;
			}
			if(DXL_Ready_Flag)
			{
				if(Motor_Error_Flag)
				{
					Send_Buffer[5] = 0x02;
					Motor_Error_Flag = 0;
				}else{
					for(j = 0;j < Motor_Length_Sign;j++)//�˴�Ӧ���Գ��ȱ߽�
					{
						Send_Buffer[i] = Motor_Buffer[j];
						i++;	
					}
				}
	
				Motor_Length_Sign = 4;
				Motor_Read_Cmd = 0;
				Motor_Read_Flag = 0;
				DXL_Ready_Flag = 0;
	
			}
		
		}
		
		
		Send_Buffer[2] = (i-2)& 0xff; // �˴�Ĭ�ϳ��Ȳ�����255,������
		Send_Buffer[3] = (i-2)>> 8;
//		Send_Buffer[3] = 0x01;
		//ȡ���鳤����CRCУ��
		CRC16 = update_crc(0, Send_Buffer, i);
		CRC_L = CRC16 & 0xff;
		CRC_H = CRC16 >> 8;
//		DXL_SendData(&i,1);
			
		Send_Buffer[i] =  CRC_L;
		i++;
		Send_Buffer[i] =  CRC_H;
		Send_BufferLen = i+1;
//		Send_Buffer[2] = 386 & 0xff;
//		Send_Buffer[3] = 386 >> 8;
//		Send_Buffer[0] = 0xff;
//		Send_Buffer[1] = 0xfa;
//		Send_Buffer[4] = 0x55;
//		Send_Buffer[5] = 0x00;
//		Send_Buffer[6] = 0x00;
//	    for(i = 7;i<390;i++)
//		{Send_Buffer[i] = 0xac;}
		USB2UART_SendData(Send_Buffer,Send_BufferLen);
//		DXL_SendData(Send_Buffer,Send_BufferLen);
//		memset(Send_Buffer,0,i);
}
	
}
