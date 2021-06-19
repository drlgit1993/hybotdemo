#include "app_mem.h"

#define MSG_OUT_MAX      (1024)
#define ESP32_OUT_MAX      (1024)

static Que_t Que_table[APP_MEM_LEN]={0};

Que_t *App_Mem_Get(APP_MEM NUM)
{
  return (&Que_table[NUM]);
}

void App_Mem_Init(void)
{
   CreateQue("DISK NMEA INPUT",&Que_table[DISK_NMEA_INPUT],USART1_RX_MAX);

   CreateQue("ESP32 INPUT",&Que_table[ESP32_INPUT],UART4_RX_MAX);

   CreateQue("AIR724 INPUT",&Que_table[AIR724_INPUT],USART2_RX_MAX);

   CreateQue("USB INPUT",&Que_table[USB_INPUT],USART6_RX_MAX);

   CreateQue("MSG OUTPUT",&Que_table[MSG_OUTPUT],MSG_OUT_MAX);
   AIR724_output_linkptr(AIR724_SERVER_MSG,&Que_table[MSG_OUTPUT]);

   CreateQue("ESP32 OUTPUT",&Que_table[ESP32_OUTPUT],ESP32_OUT_MAX); 
}
