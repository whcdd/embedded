#ifndef _DYNAMIXEL_MOTOR_H
#define _DYNAMIXEL_MOTOR_H

#include "config.h"


/* External variables --------------------------------------------------------*/
#define DXL_RXBUFFSIZE 400

extern UART_HandleTypeDef huart1;
//extern u8 DXL_aRxBuffer[DXL_RXBUFFSIZE];
extern __align(4) u8 DXL_aRxBuffer[DXL_RXBUFFSIZE] __attribute__((at(0XC0000500)));
//extern u8 DXL_aTxBuffer[DXL_RXBUFFSIZE];
extern DMA_HandleTypeDef hdma_uart1_tx;
extern DMA_HandleTypeDef hdma_uart1_rx;
/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  Instruction definition  
  */  
typedef enum
{
    Ping          = 0x01,      /*!<  Instruction that checks whether the Packet has arrived to a device with the same ID as Packet ID                           */
	  Read          = 0x02,      /*!<  Instruction to read data from the Device                                                                                   */
	  Write         = 0x03,      /*!<  Instruction to write data on the Device                                                                                    */      
    Reg_Write     = 0x04,      /*!<  Instruction that registers the Instruction Packet to a standby status; Packet is later executed through the Action command  */
	  Action        = 0x05,      /*!<  Instruction that executes the Packet that was registered beforehand using Reg Write                                        */
    Factory_Reset = 0x06,      /*!<  Instruction that resets the Control Table to its initial factory default settings                                          */
    Reboot        = 0x08,      /*!<  Instruction to reboot the Device                                                                                           */
	  Clear         = 0x10,      /*!<  Instruction to reset certain information                                                                                   */
    Status        = 0x55,      /*!<  Return Instruction for the Instruction Packet                                                                              */
    Sync_Read     = 0x82,      /*!<  For multiple devices, Instruction to read data from the same Address with the same length at once                          */
	  Sync_Write    = 0x83,      /*!<  For multiple devices, Instruction to write data on the same Address with the same length at once                           */
	  Bulk_Read	    = 0x92,      /*!<  For multiple devices, Instruction to read data from different Addresses with different lengths at once                     */
	  Bulk_Write    = 0x93       /*!<  For multiple devices, Instruction to write data on different Addresses with different lengths at once                      */
}InstructionTypeDef;

/** 
  * @brief  Error definition that indicates the processing result of Instruction Packet
  */  
typedef enum
{
    Result_Fail         = 0x01,     /*!<  Failed to process the sent Instruction Packet                                                                         */
	  Instruction_Error   = 0x02,     /*!<  Undefined Instruction has been used;Action has been used without Reg Write                                            */
	  CRC_Error           = 0x03,     /*!<  CRC of the sent Packet does not match                                                                                 */
	  Data_Range_Error	  = 0x04,     /*!<  Data to be written in the corresponding Address is outside the range of the minimum/maximum value                     */
	  Data_Length_Error	  = 0x05,     /*!<  Attempt to write Data that is shorter than the data length of the corresponding Address
	                                        (ex: when you attempt to only use 2 bytes of a item that has been defined as 4 bytes)                                 */
	  Data_Limit_Error    = 0x06,     /*!<  Data to be written in the corresponding Address is outside of the Limit value                                         */
		Access_Errer        = 0x07      /*!<  Attempt to write a value in an Address that is Read Only or has not been defined
                                          Attempt to read a value in an Address that is Write Only or has not been defined
                                          Attempt to write a value in the ROM domain while in a state of Torque Enable(ROM Lock)                                */
}ErrorStateTypeDef;

/** 
  * @brief  Access Definition
  */  
typedef enum
{
    R           = 0x01,     /*!<  Read only                                                                        */
	  RW          = 0x02,     /*!<  Read and write                                                                   */

}AccessTypeDef;

/** 
  * @brief  The control Table Definition of Motor containing address,size,data name,access type,default,range
  */ 
typedef struct
{
    u16  address;                 /*!<  The Address is a unique value when accessing a specific Data in the Control Table with Instruction Packets.   */
	  u8 size;                      /*!<  The Size of data varies from 1 ~ 4 bytes depend on their usage.                                               */
//	  unsigned char data_name[30];  /*!<  The Control Table has two different access properties.                                                      */
	  AccessTypeDef access;         /*!<  The Control Table has two different access properties.                                                        */
	  u16 defval;				            /*!<  Each data in the Control Table is restored to initial values when the device is turned on. 
	                                      Default values in the EEPROM area are initial values of the device (factory default settings).                */
	  int range_L;                  /*!<  The valid range of the parameter                                                                              */
		int range_H;                  /*!<  The valid range of the parameter                                                                              */
}CtrlTableTypeDef;

/** 
  * @brief  DYNAMIXEL Protocol 2.0 Structure of Instruction Packet which is the command data sent to the Device.
  */
typedef struct
{
    u8 id;                       /*!<  The ID of the Device that should receive the Instruction Packet and process its                                             */
	  u16 len;                     /*!<  The length after the Packet Length field (Instruction, Parameter, CRC fields). Packet Length = number of Parameters + 3     */  
    InstructionTypeDef Ins;      /*!<  The field that defines the type of command                                                                                  */
	  u8 *Param;                   /*!<  As the auxiliary data field for Instruction, its purpose is different for each Instruction.                                 */
	  ErrorStateTypeDef Err;       /*!<  The field that defines the type of error                                                                                    */
//	  u16 mycrc;                   /*!<  16bit CRC field checks if the Packet has been damaged during communication.                                                 */
	  CtrlTableTypeDef *CtrlTable; /*!<  The control Table of Motor containing address,size,data name,access type,default,range */
}DXL_HandlerTypeDef;



/** 
  * @brief  DXL Status structures definition  
  */  
typedef enum 
{
  DXL_OK       = 0x00U,
  DXL_ERROR    = 0x01U
} DXL_StatusTypeDef;

/** 
  * @brief  DXL Status structures definition  
  *  DE: Sending data enable   RE: Receiving data enable
  */  
typedef enum 
{
  RE    = 0x00U,
  DE    = 0x01U
} DXL_DIRTypeDef;




/** @defgroup DXL direction define
  * @{ DE: Sending data enable   RE: Receiving data enable
  */
void DXL_DIR(DXL_DIRTypeDef DIR);

DXL_StatusTypeDef DXLMotor_Init(u32 baud);

DXL_StatusTypeDef DXL_SendData(u8 *databuf, u8 len);
DXL_StatusTypeDef DXL_ReceiveData(u8 *datades,u8 len);


//DXL_StatusTypeDef DXL_ProtocolSendData(DXL_HandlerTypeDef *DXL_Handler);
//DXL_StatusTypeDef DXL_ProtocolReceiveData(DXL_HandlerTypeDef *DXL_Handler);
static void DXL_ReceiverTimeoutInit(void);
static void DXL_DMA_Init(void);
void DXL_UART_Transmit(u8 *databuf, u8 len);


#endif 
