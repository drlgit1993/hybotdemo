/**
  * @file    bsp_led.c
  * @author  DRL
  * @version V1.0.0
  * @date    29-3-2021
  * @brief   
  *          	
**/
#include "bsp_led.h"


void bsp_Led_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   LED_SYS_CLK_ENABLE();

   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Pin = LED_SYS_PIN;
   HAL_GPIO_Init(LED_SYS_PORT, &GPIO_InitStruct);

	  LED_SYS_OFF();

   __HAL_RCC_GPIOB_CLK_ENABLE();

   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Pin = GPIO_PIN_4;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
}
