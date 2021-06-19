#include "bsp_power.h"

void Sys_Power_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   BAT_ON_CLK_ENABLE();

   /*battery ON*/
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Pin = BAT_ON_PWR_PIN;
   HAL_GPIO_Init(BAT_ON_PWR_PORT, &GPIO_InitStruct);

   HAL_GPIO_WritePin(BAT_ON_PWR_PORT,BAT_ON_PWR_PIN,GPIO_PIN_SET);

   /*battery OFF*/
   BAT_OFF_CLK_ENABLE();
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pin = BAT_OFF_PWR_PIN;
   HAL_GPIO_Init(BAT_OFF_PWR_PORT, &GPIO_InitStruct); 
   HAL_NVIC_SetPriority(BAT_OFF_EXTI_IRQ, 1, 0);
   HAL_NVIC_EnableIRQ(BAT_OFF_EXTI_IRQ);
}

void BAT_OFF_IRQHandler(void)
{
	  if(__HAL_GPIO_EXTI_GET_IT(BAT_OFF_PWR_PIN) != RESET) 
	  {
       HAL_GPIO_WritePin(BAT_ON_PWR_PORT,BAT_ON_PWR_PIN,GPIO_PIN_RESET);
		     __HAL_GPIO_EXTI_CLEAR_IT(BAT_OFF_PWR_PIN);     
	  }  
}
