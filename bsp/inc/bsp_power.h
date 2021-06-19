#ifndef _BSP_POWER_H
#define _BSP_POWER_H

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif


#define BAT_ON_PWR_PORT				            GPIOC
#define BAT_ON_PWR_PIN				             GPIO_PIN_3
#define BAT_ON_CLK_ENABLE()				        __HAL_RCC_GPIOC_CLK_ENABLE()


#define BAT_OFF_PWR_PORT				            GPIOC
#define BAT_OFF_PWR_PIN				             GPIO_PIN_1
#define BAT_OFF_CLK_ENABLE()				        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BAT_OFF_EXTI_IRQ                EXTI1_IRQn
#define BAT_OFF_IRQHandler              EXTI1_IRQHandler

void Sys_Power_Init(void);

#ifdef __cplusplus
}
#endif

#endif
