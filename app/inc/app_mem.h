#ifndef _APP_MEM_H
#define _APP_MEM_H

#include "que.h"
#include "bsp_usart1.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
   NMEA_INPUT,
   DISK_NMEA_INPUT,
   ESP32_INPUT,
   AIR724_INPUT,
   USB_INPUT,
   MSG_OUTPUT,
   ESP32_OUTPUT,
   APP_MEM_LEN
}APP_MEM;

Que_t *App_Mem_Get(APP_MEM NUM);
void App_Mem_Init(void);

#ifdef __cplusplus
}
#endif

#endif
