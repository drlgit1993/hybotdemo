/**
  * @file    bsp_led.h
  * @author  DRL
  * @version V1.0.0
  * @date    29-3-2021
  * @brief   
  *          	
**/
#ifndef _BSP_LED_H
#define _BSP_LED_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
#include "main.h"

#define LED_SYS_PORT				           GPIOA
#define LED_SYS_PIN				            GPIO_PIN_15
#define LED_SYS_CLK_ENABLE()				   __HAL_RCC_GPIOA_CLK_ENABLE()

#define LED_SYS_ON()				           (LED_SYS_PORT->BSRR =(uint32_t)LED_SYS_PIN << 16)
#define LED_SYS_OFF()				          (LED_SYS_PORT->BSRR =LED_SYS_PIN)
#define LED_SYS_TOGGLE()	 	        (LED_SYS_PORT->ODR ^=LED_SYS_PIN)
#define LED_SYS_DEFAULT()          (LED_SYS_OFF())


void bsp_Led_Init(void);


#ifdef __cplusplus
}
#endif

#endif	/*_BSP_LED_H*/
