/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      commandProcess.c                                                                                            ***/
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
#include "commandProcess.h"


/* External variables -----------------------------------------------------------------------------------------------------------*/
u8 MCU_Read_Flag = 0;
u8 IMU_Read_Flag = 0;
u8 Left_Read_Flag = 0;
u8 Right_Read_Flag = 0;
u8 Motor_Read_Flag = 0;
u8 Motor_Read_Cmd = 0;
u16 Sync_Read_Len = 0;
u16 Bulk_Read_Len = 0;
//u8 Motor_Length_Sign = 3; //加上组帧后3个字节包头长度


/* Private function prototypes --------------------------------------------------------------------------------------------------*/
/** @addtogroup commandProcess_Private_Functions
  * @{
  */
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

/*********************************************************************************************************************************/
/**Function Name:         Data_Receive                                                                                        **/
/**Function Description:  收到的数据初步解析提取各个设备的命令                                                                    **/
/**Inputs:                u8 *InputBuffer                                                                                       **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Data_Receive(void)
{
	 if(UART4RxEndFlag)
	{
//		DXL_SendData(USB2UART_aRxBuffer0,UART4RxLen);	
		Cmd_Distribute(USB2UART_aRxBuffer0);	//此时用的是发送缓存；调试后需更改
		
		/* Progress the reveiving data */
//		USB2UART_SendData(test1,sizeof(test1)/sizeof(u8));
//		USB2UART_SendData(USB2UART_aRxBuffer0,UART4RxLen);
//		DXL_SendData(test1,30);
//		DXL_SendData(USB2UART_aTxBuffer0,UART4RxLen);
		UART4RxEndFlag = 0;
		UART4RxLen = 0;
	}
}

/*********************************************************************************************************************************/
/**Function Name:         Cmd_Distribute                                                                                        **/
/**Function Description:  收到的数据初步解析提取各个设备的命令                                                                    **/
/**Inputs:                u8 *InputBuffer                                                                                       **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Cmd_Distribute(u8 *InputBuffer)
{
	u8 i = 0;
	u8 length;
	u8 length_H = 0;
	u8 *cmdBuffer;
	u8 cmdLength;
	u8 CRC_L=0,CRC_H=0;
	u16 CRC16=0;
	u8 error_Packet[] = {0xff,0xfa,0x04,0x00,0x55,0x01,0x43,0x2f};//CRC待校验
	//test
//	u8 headerr[] = {0xAB,0xCD};
//	u8 lengtherr[] = {0xAC,0xBF};
//	u8 crcerr[] = {0xBC,0xDE};
//	//CRC校验
//	CRC16 = update_crc(0, error_Packet,6);
//	CRC_L = CRC16 & 0xff;
//    CRC_H = CRC16 >> 8;
//	
//	error_Packet[6] = CRC_L;
//	error_Packet[7] = CRC_H;
	
//	length_H = (InputBuffer[3])<<8;
	length = InputBuffer[2] +length_H;
	cmdLength = length-3;
	//包头校验
//	USB2UART_SendData(InputBuffer,length+4);
	if((InputBuffer[0] != 0xFF) || (InputBuffer[1] != 0xFA))
	{
		//向上位机返回包头校验错误字
//		USB2UART_SendData(headerr,2);//test
		USB2UART_SendData(error_Packet,8);
//		DXL_SendData(USB2UART_aRxBuffer0,sizeof(USB2UART_aRxBuffer0)/sizeof(u8));
		return;
	}
	if( length != (UART4RxLen-4))
	{
		//向上位机返回长度校验错误字
//		DXL_SendData(lengtherr,2);//test
		USB2UART_SendData(error_Packet,8);
		return; 
	}
	
	//CRC校验
	CRC16 = update_crc(0, InputBuffer,UART4RxLen-2);
	CRC_L = CRC16 & 0xff;
    CRC_H = CRC16 >> 8;
		
	if((InputBuffer[UART4RxLen-2] != CRC_L) || (InputBuffer[UART4RxLen-1] != CRC_H))
	{
//		DXL_SendData(crcerr,2);//test
		USB2UART_SendData(error_Packet,8);
		//向上位机返回CRC校验错误字
		return;
	}	
		
	cmdBuffer = (u8 *)malloc(cmdLength*sizeof(u8));
	memset(cmdBuffer,0,cmdLength);
	for(i = 0;i<cmdLength;i++)//不包含CRC校验字
	{
		cmdBuffer[i] = InputBuffer[i+5];
	}
	
//	USB2UART_SendData(cmdBuffer,cmdLength);
//	DXL_SendData(cmdBuffer,cmdLength); //测试用
	Cmd_Choose(InputBuffer[4],cmdBuffer,cmdLength);//传递去除包头数据的裸包
	free(cmdBuffer);
}

/*********************************************************************************************************************************/
/**Function Name:         Cmd_Choose                                                                                            **/
/**Function Description:  命令类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Cmd_Choose(u8 cmdNum,u8 *cmdWord,u8 cmdLength)
{
	switch(cmdNum)
	{
		case 0x93:
			Config_Send(cmdWord,cmdLength);
//			DXL_SendData(cmdWord,cmdLength);
			break;
		case 0x92:
//			DXL_SendData(cmdWord,cmdLength);
			Req_Send(cmdWord,cmdLength);
			break;
		default:
			//向上位机返回错误字
			return;
	}
}

/*********************************************************************************************************************************/
/**Function Name:         Config_Send                                                                                           **/
/**Function Description:  发送配置                                                                                              **/
/**Inputs:                u8 *InputBuffer                                                                                       **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Config_Send(u8 *InputBuffer,u8 cmdLength)
{
	u8 length1,length2,length3,length4,length5;
//	u16 cmd_L,cmd_H;
	u8 devNum1,devNum2,devNum3,devNum4,devNum5;
	u8 *devBuffer,*InputBuffer2;
	u8 currentLength1 = 0,currentLength2 = 0,currentLength3 = 0,currentLength4 = 0,currentLength5 = 0;
	u8 i= 0,j = 0;
	
	//判断第一个命令字
	devNum1 = InputBuffer[0];
//	cmd_L = InputBuffer[1];
//	cmd_H = InputBuffer[2];
	length1 = InputBuffer[3];
//	DXL_SendData(InputBuffer,cmdLength); //测试用
	currentLength1 = length1 + 4; //单个设备命令总长度
//	DXL_SendData(InputBuffer,cmdLength);
	if(currentLength1  <= cmdLength)
	{
		devBuffer = (u8 *)malloc(currentLength1*sizeof(u8));
		memset(devBuffer,0,currentLength1);
		for(i = 0;i < currentLength1;i++)
		{
			devBuffer[i] = InputBuffer[i];
		} 
		for(i = 0;i < cmdLength;i++)
		{
			InputBuffer2[i] = InputBuffer[i];
		}
//		DXL_SendData(InputBuffer2,cmdLength);
		Device_Choose(devNum1,devBuffer,length1);
		free(devBuffer);
//		DXL_SendData(devBuffer,currentLength1);
	}
	
	//判断第二个命令字
	if(currentLength1  < cmdLength)//判断第二个设备命令字是否存在
	{
		j = currentLength1;
		devNum2 = InputBuffer[j];
		length2 = InputBuffer[j+3]; //单个设备命令总长度
		currentLength2 = length2 + 4;
		devBuffer = (u8 *)malloc(currentLength2*sizeof(u8));
		memset(devBuffer,0,currentLength2);
//		DXL_SendData(InputBuffer,cmdLength); //测试用
		for(i = 0;i < currentLength2;i++)
		{
			devBuffer[i] = InputBuffer[i+j];
		}
//		DXL_SendData(devBuffer,currentLength2); //测试用
//			DXL_SendData(InputBuffer,30); //测试用
		Device_Choose(devNum2,devBuffer,length2);
		free(devBuffer);
	}		
	
	//判断第三个命令字
	if((currentLength1 + currentLength2) < cmdLength)//判断第二个设备命令字是否存在
	{
		j = currentLength1 + currentLength2;
		devNum3 = InputBuffer[j];
		length3 = InputBuffer[j+3]; //单个设备命令总长度
		currentLength3 = length3 + 4;
		devBuffer = (u8 *)malloc(currentLength3*sizeof(u8));
		memset(devBuffer,0,currentLength3);
		for(i = 0;i < currentLength3;i++)
		{
			devBuffer[i] = InputBuffer[i+j];
		}
		Device_Choose(devNum3,devBuffer,length3);
		free(devBuffer);
	}
	
	//判断第四个命令字
	if((currentLength1 + currentLength2 + currentLength3) < cmdLength)//判断第二个设备命令字是否存在
	{
		j = currentLength1 + currentLength2 + currentLength3;
		devNum4 = InputBuffer[j];
//		j = j+3;
		length4 = InputBuffer[j+3]; //单个设备命令总长度
//		DXL_SendData(&length4,1);
		currentLength4 = length4 + 4;
		devBuffer = (u8 *)malloc(currentLength4*sizeof(u8));
		memset(devBuffer,0,currentLength4);
		for(i = 0;i < currentLength4;i++)
		{
			devBuffer[i] = InputBuffer[i+j];
		}
		Device_Choose(devNum4,devBuffer,length4);
		free(devBuffer);
	}
	//判断第五个命令字
	if((currentLength1 + currentLength2 + currentLength3 + currentLength4) < cmdLength)//判断第二个设备命令字是否存在
	{
		j = currentLength1 + currentLength2 + currentLength3 + currentLength4;
		devNum5 = InputBuffer[j];
		length5 = InputBuffer[j+3]; //单个设备命令总长度
		currentLength5 = length5 + 4;
//		DXL_SendData(&length5,1);
		devBuffer = (u8 *)malloc(currentLength5*sizeof(u8));
		memset(devBuffer,0,currentLength5);
		for(i = 0;i < currentLength5;i++)
		{
			devBuffer[i] = InputBuffer[i+j];
		}
		Device_Choose(devNum5,devBuffer,length5);
		free(devBuffer);
	}
	
	
}
/*********************************************************************************************************************************/
/**Function Name:         Req_Send                                                                                              **/
/**Function Description:  发送读取请求                                                                                           **/
/**Inputs:                u8 *InputBuffer                                                                                       **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Req_Send(u8 *InputBuffer,u8 cmdLength)
{
//	u8 length,length2,length3,length4,length5;
	u8 devNum1,devNum2,devNum3,devNum4;
//	u8 *motorBuffer;
	u8 currentDev;
	u8 com_L,com_H;
	u8 i= 0;
//	u8 test1[] = {0x12,0x34,0x56};
	
	//判断第一个命令字
	currentDev = InputBuffer[i];
	//当设备为舵机时；
	if(currentDev == 0x05)
	{
		Motor_Data_Read(InputBuffer,i);
//		DXL_SendData(InputBuffer,cmdLength);
		return;
	}
	devNum1 = currentDev;
	i++;
	com_L = InputBuffer[i];
	i++;
	com_H = InputBuffer[i];
	Read_Dev_Choose(devNum1,com_L,com_H);
	if(cmdLength == 3) //1个读请求
	{
		return;
	}
	
	i++;
	currentDev = InputBuffer[i];
	if(currentDev == 0x05)
	{
		Motor_Data_Read(InputBuffer,i);
		return;
	}
	
	devNum2 = currentDev;
	i++;
	com_L = InputBuffer[i];
	i++;
	com_H = InputBuffer[i];
	Read_Dev_Choose(devNum2,com_L,com_H);
	if(cmdLength == 6) //2个读请求
	{
		return;
	}
	
	i++;
	currentDev = InputBuffer[i];
	if(currentDev == 0x05)
	{
		Motor_Data_Read(InputBuffer,i);
		return;
	}
	
	devNum3 = currentDev;
	i++;
	com_L = InputBuffer[i];
	i++;
	com_H = InputBuffer[i];
	Read_Dev_Choose(devNum3,com_L,com_H);
	if(cmdLength == 9) //3个读请求
	{
		return;
	}
	
	i++;
	currentDev = InputBuffer[i];
	if(currentDev == 0x05)
	{
		Motor_Data_Read(InputBuffer,i);
		return;
	}
	
	devNum4 = currentDev;
	i++;
	com_L = InputBuffer[i];
	i++;
	com_H = InputBuffer[i];
	Read_Dev_Choose(devNum4,com_L,com_H);
	if(cmdLength == 12) //4个读请求
	{
		return;
	}
	
	i++;
	currentDev = InputBuffer[i];
	if(currentDev != 0x05)
	{
		//返回设备号错误
		return;
	}
	Motor_Data_Read(InputBuffer,i);
}


/*********************************************************************************************************************************/
/**Function Name:         update_crc                                                                                            **/
/**Function Description:  CRC校验                                                                                               **/
/**Inputs:                unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size                   **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
    unsigned short i, j;
    unsigned short crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };

    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

/*********************************************************************************************************************************/
/**Function Name:         Device_Choose                                                                                            **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Device_Choose(u8 devNum,u8 *devCmd,u8 devCmdLen)
{
	switch(devNum)
	{
		case 0x01:
			MCU_cmd_Send(devCmd,devCmdLen);
			break;
		case 0x02:
			IMU_cmd_Send(devCmd,devCmdLen);
			break;
		case 0x03:
			Left_cmd_Send(devCmd,devCmdLen);
			break;
		case 0x04:
			Right_cmd_Send(devCmd,devCmdLen);
			break;
		case 0x05:
//			DXL_SendData(devCmd,devCmdLen);
			Motor_cmd_Send(devCmd,devCmdLen);
			break;
		default:
			//向上位机返回设备命令错误字
			return;
	}
}

/*********************************************************************************************************************************/
/**Function Name:         MCU_cmd_Send                                                                                          **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void MCU_cmd_Send(u8 *cmdWord,u8 cmdLength)
{
	u8 *DXL_buffer;
	u8 i = 0;
	for(i = 0;i<cmdLength;i++)
	{
		DXL_buffer[i] = cmdWord[i+4];
	}
	DXL_SendData(DXL_buffer,cmdLength);
}
/*********************************************************************************************************************************/
/**Function Name:         IMU_cmd_Send                                                                                          **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void IMU_cmd_Send(u8 *cmdWord,u8 cmdLength)
{
	//IMU配置命令处理
}

/*********************************************************************************************************************************/
/**Function Name:         Left_cmd_Send                                                                                         **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Left_cmd_Send(u8 *cmdWord,u8 cmdLength)
{

}

/*********************************************************************************************************************************/
/**Function Name:         Right_cmd_Send                                                                                        **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Right_cmd_Send(u8 *cmdWord,u8 cmdLength)
{

}

/*********************************************************************************************************************************/
/**Function Name:         Motor_cmd_Send                                                                                        **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Motor_cmd_Send(u8 *cmdWord,u8 cmdLength)
{
	u8 *DXL_buffer;
	u8 i = 0;
	for(i = 0;i<cmdLength;i++)
	{
		DXL_buffer[i] = cmdWord[i+4];
	}
	DXL_SendData(DXL_buffer,cmdLength);
}

/*********************************************************************************************************************************/
/**Function Name:         Motor_Data_Read                                                                                        **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Motor_Data_Read(u8 *cmdWord,u8 currentPoint)
{
	u8 j = 0;
	u8 length,bulkReadNum;
	u8 com_L,com_H;
	u8 *motorBuffer;
//	u8 *transPoint;
	u8 transP;
	u8 sinLength;
	
	currentPoint++;
	com_L = cmdWord[currentPoint];
	Motor_Read_Cmd = com_L;
//	DXL_SendData(&Motor_Read_Cmd,1);
	currentPoint++;
//	com_H = cmdWord[currentPoint];
	currentPoint++;
	length = cmdWord[currentPoint];
	currentPoint++;
	switch(Motor_Read_Cmd)
	{
		case 0x82://Sync Read
			Sync_Read_Len = length - 14;
		    transP = currentPoint + 10;
			sinLength = 11 + cmdWord[transP];
			Sync_Read_Len = sinLength * Sync_Read_Len;
//		DXL_SendData(&Sync_Read_Len,1);
			break;
		case 0x92://Bulk Read
			bulkReadNum = ((length - 10)/5);
			transP = currentPoint + 11;
//			transPoint = &cmdWord[transP];
			for(j = 0;j<bulkReadNum;j++)
			{
				sinLength = 11 + cmdWord[transP];
				Bulk_Read_Len += sinLength;
				transP += 5;
			}
			
//			DXL_SendData(&Bulk_Read_Len,1);
			break;
		default:
			//向上位机返回设备命令错误字
//			transPoint = NULL;
			break;
	}
	motorBuffer = (u8 *)malloc(length*sizeof(u8));
	memset(motorBuffer,0,length);
	for(j = 0;j<length;j++)
	{
		motorBuffer[j] = cmdWord[j+currentPoint];
		
	}
	DXL_SendData(motorBuffer,length);
	free(motorBuffer);
	Motor_Read_Flag = 1;
}


/*********************************************************************************************************************************/
/**Function Name:         Read_Dev_Choose                                                                                            **/
/**Function Description:  设备类型判断                                                                                           **/
/**Inputs:                u8 cmdNum,u8 *cmdWord,u8 cmdLength                                                                    **/
/**                                                                                                                             **/
/**Outputs:               void                                                                                                  **/
/**                                                                                                                             **/
/**Notes:                                                                                                                       **/
/*********************************************************************************************************************************/
void Read_Dev_Choose(u8 devNum,u8 Cmd_L,u8 Cmd_H)
{
	switch(devNum)
	{
		case 0x01:
			MCU_Read_Flag = 1;
			break;
		case 0x02:
			IMU_Read_Flag = 1;
			break;
		case 0x03:
			Left_Read_Flag = 1;
			break;
		case 0x04:
			Right_Read_Flag = 1;
			break;
		default:
			//向上位机返回设备命令错误字
			return;
	}
}
