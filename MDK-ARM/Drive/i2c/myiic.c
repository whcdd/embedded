/*********************************************************************************************************************************/
/**********************************zhejianglab************************************************************************************/
/***FileName:      myiic.c                                                                                                      ***/
/***Author:                                                                                                                    ***/
/***Version:       1.0                                                                                                         ***/
/***Date:                                                                                                                      ***/ 
/***Description:   IIC����                                                                                                      ***/
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
#include "myiic.h"

//IIC��ʼ��
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOHʱ��
    
    //PB10,11��ʼ������
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_NOPULL;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    IIC_SDA(1);
    IIC_SCL(1);  
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA(1);	  	  
	IIC_SCL(1);
	delay_us(4);
 	IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL(0);
	IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL(1); 
	delay_us(4);			
	IIC_SDA(1);//����I2C���߽����ź�				   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;	
	SDA_IN();      //SDA����Ϊ���� 
	IIC_SDA(1);delay_us(1);	   
	IIC_SCL(1);delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	delay_us(2);
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		delay_us(2);
		IIC_SCL(1);
		delay_us(2); 
		IIC_SCL(0);	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL(0); 
        delay_us(2);
		IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

/*******************************************************************************
* Function Name  : I2C_LM75_Read
* Description    : Reads Temperature data from the LM75.
* Input          : None
* Output         : 0xFFFF -- Error
*					0x0000 ~ 0x01FF -- Valid data
*					Temp	Binary 					Hex
*					+125	0111 1101 0xxx xxxx 	0FAh
*					+25		0001 1001 0xxx xxxx 	032h
*					+0.5	0000 0000 1xxx xxxx 	001h
*					0		0000 0000 0xxx xxxx		000h
*					-0.5	1111 1111 1xxx xxxx 	1FFh
*					-25		1110 0111 0xxx xxxx 	1CEh
*					-55		1100 1001 0xxx xxxx 	192h
* Return         : None
*******************************************************************************/
//LM75BD���¶�
float I2C_LM75_Read(void)
{	
	uint16_t Temputerature_8,Temputerature_16;//�¶�ֵ��1�ν���8λ
	float Temputerature;
	//IIC������SDA1 ->0 ,delay, SCL 1->0
	//Ĭ�������¶�ģʽ
	IIC_Start();
	IIC_Send_Byte(0x90);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	
	IIC_Start();
	//����(0x91)1001 0001��1001,Ӳ�����ߣ� 0001���ӻ���ַ--000 ��ģʽ--1
	IIC_Send_Byte(0x91);
	//�ȴ�Ӧ��
	IIC_Wait_Ack();//����ֵ��1������Ӧ��ʧ��; 0������Ӧ��ɹ�
	//ACK=0--�������� 
	Temputerature_8 = IIC_Read_Byte(1);//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
	//ACK=1--�������
	Temputerature_16 = (Temputerature_8 <<8)+(IIC_Read_Byte(0));
	//IICֹͣ
	IIC_Stop();
	//ȡ�¶�������
	Temputerature_16 = Temputerature_16 >> 5;//1LSB=0.125��
	//���¶�
	if(Temputerature_16&0x0400) 
		Temputerature = -(~(Temputerature_16&0x03FF)+1)*0.125;
	//���¶�
	else 
		Temputerature = 0.125*(float)Temputerature_16;
	//�����¶�ֵ 1LSB=0.01��
//	return ((u16)(Temputerature*100));
	return Temputerature;
}
