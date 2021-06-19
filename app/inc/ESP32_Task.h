#ifndef _ESP32_TASK_H
#define _ESP32_TASK_H

#include "main.h"
#include "app_mem.h"

#ifdef __cplusplus
extern "C"{
#endif



#define BLE_WIFI_PWR_PORT				           GPIOC
#define BLE_WIFI_PWR_PIN				            GPIO_PIN_13
#define BLE_WIFI_PWR_CLK_ENABLE()				   __HAL_RCC_GPIOC_CLK_ENABLE()

#define BLE_WIFI_PWR_ON()				           (BLE_WIFI_PWR_PORT->BSRR =(uint32_t)BLE_WIFI_PWR_PIN << 16)
#define BLE_WIFI_PWR_OFF()				          (BLE_WIFI_PWR_PORT->BSRR =BLE_WIFI_PWR_PIN)


#define BLE_WIFI_EN_PORT				           GPIOC
#define BLE_WIFI_EN_PIN				            GPIO_PIN_15
#define BLE_WIFI_EN_CLK_ENABLE()				   __HAL_RCC_GPIOC_CLK_ENABLE()

#define BLE_WIFI_EN_ON()				           (BLE_WIFI_PWR_PORT->BSRR =(uint32_t)BLE_WIFI_PWR_PIN << 16)
#define BLE_WIFI_EN_OFF()				          (BLE_WIFI_PWR_PORT->BSRR =BLE_WIFI_PWR_PIN)


#define ESP32_EVENT_WIFI_SCAN    (1<<0)
#define ESP32_EVENT_BLE_OUT      (1<<1)
#define ESP32_EVENT_BLE_RECV     (1<<2)
#define ESP32_EVENT_UART_RECV    (1<<3)

#define ESP32_EVENT_ALL    (ESP32_EVENT_SCAN|ESP32_EVENT_OUT)


void ESP32_Task_Create(void);
void ESP32_Receive_Set(void);

#ifdef __cplusplus
}
#endif


#endif
