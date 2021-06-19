#ifndef _USART_TASK_H
#define _USART_TASK_H

#include "bsp_usart.h"
#include "bsp_usart1.h"
#include "bsp_flash.h"
#include "syscfg.h"
#include "bsp_i2c.h"
#include "xsz_protocol.h"

#ifdef __cplusplus
extern "C"{
#endif


void USART6_Task_Create(void);
void USART6_Receive_Set(void);



#ifdef __cplusplus
}
#endif

#endif
