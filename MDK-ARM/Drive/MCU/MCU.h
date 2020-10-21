#ifndef _MCU_H
#define _MCU_H
#include "config.h"
#include "myiic.h"

extern u8 Emerge_flag;
extern u8 Under_Voltage_flag;
extern u8 Over_Voltage_flag;

void Fan_Init(void);
void Bat_Protect_Init(void);
void Battery_Protect(void);
void Emerge_Stop_Init(void);
void Emerge_Stop_Monitor(void);
void Fan_Monitor(void);

#endif
