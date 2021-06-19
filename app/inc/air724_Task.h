#ifndef _AIR724_TASK_H
#define _AIR724_TASK_H

#include "main.h"
#include "log.h"
#include "bsp_led.h"

#ifdef __cplusplus
extern "C"{
#endif


#define AIR724_INIT_OK     (1)
#define AIR724_INIT_ERR    (0)


#define AIR724_PWRKEY_PORT				             GPIOC
#define AIR724_PWRKEY_PIN				              GPIO_PIN_14
#define AIR724_PWRKEY_CLK_ENABLE()				     __HAL_RCC_GPIOC_CLK_ENABLE()
#define AIR724_PWRKEY_ON()				             (AIR724_PWRKEY_PORT->BSRR =(uint32_t)AIR724_PWRKEY_PIN << 16)
#define AIR724_PWRKEY_OFF()				            (AIR724_PWRKEY_PORT->BSRR =AIR724_PWRKEY_PIN)


#define AIR724_RST_PORT				                GPIOB
#define AIR724_RST_PIN				                 GPIO_PIN_6
#define AIR724_RST_CLK_ENABLE()				        __HAL_RCC_GPIOB_CLK_ENABLE()
#define AIR724_RST_ON()				                (AIR724_RST_PORT->BSRR =(uint32_t)AIR724_RST_PIN << 16)
#define AIR724_RST_OFF()				               (AIR724_RST_PORT->BSRR =AIR724_RST_PIN)

#define AIR724_PWR_PORT				                GPIOB
#define AIR724_PWR_PIN				                 GPIO_PIN_4
#define AIR724_PWR_CLK_ENABLE()				        __HAL_RCC_GPIOB_CLK_ENABLE()
#define AIR724_PWR_OFF()				                (AIR724_PWR_PORT->BSRR =(uint32_t)AIR724_PWR_PIN << 16)
#define AIR724_PWR_ON()				               (AIR724_PWR_PORT->BSRR =AIR724_PWR_PIN)

#define SERVER_MSG_Pos                     (0UL)                                           
#define SERVER_MSG_Msk                     (0x1UL << SERVER_MSG_Pos)

#define SERVER_OTA_Pos                     (1UL)                                           
#define SERVER_OTA_Msk                     (0x1UL << SERVER_OTA_Pos)

                                        
#define SERVER_ALL_Msk                     (SERVER_MSG_Msk|SERVER_OTA_Msk)

//#define MSG_INITED_EVENT                   (0x01 << 0)
//#define OTA_INITED_EVENT                   (0x01 << 1)
//#define MSG_NORMAL_SEND_EVENT              (0x01 << 2)
//#define MSG_BACKUP_SEND_EVENT              (0x01 << 3)
//#define OTA_SEND_EVENT                     (0x01 << 4)

#define AIR724_EVENT_UART_RECV             (1<<0)
#define AIR724_EVENT_ACK                   (1<<1)
#define AIR724_EVENT_RECV                  (1<<2)
#define AIR724_EVENT_SEND_NORMAL           (1<<3)
#define AIR724_EVENT_SEND_BACKUP           (1<<4)

typedef enum  
{
  AIR724_SERVER_MSG,
  AIR724_SERVER_OTA,
  AIR724_SERVER_NUM
}AIR724_SERVER;

#define IS_AIR724_SERVER(SERVER)     ((SERVER==AIR724_SERVER_MSG)||(SERVER==AIR724_SERVER_OTA))

void AIR724_Task_Create(void);
void AIR724_Receive_Set(void);
void AIR724_output_linkptr(AIR724_SERVER server,Que_t *que);

#ifdef __cplusplus
}
#endif

#endif

