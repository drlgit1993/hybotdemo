#include "lps22hb.h"



static void LPS22HB_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   LPS22HB_INT2_CLK_ENABLE();

   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pin = LPS22HB_INT2_PIN;
   HAL_GPIO_Init(LPS22HB_INT2_PORT, &GPIO_InitStruct); 
   HAL_NVIC_SetPriority(LPS22HB_INT2_EXTI_IRQ, 5, 0);
   HAL_NVIC_EnableIRQ(LPS22HB_INT2_EXTI_IRQ);
}
void LPS22HB_Test(void)
{
   uint8_t i=0,len=0,reg_data=0;
   uint8_t LPS22HB_REG_ADDR[]={0x0b,0x0c,0xd,0x0f,0x10,0x11,0x12,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x33};

   len=sizeof(LPS22HB_REG_ADDR)/sizeof(uint8_t);
   for(i=0;i<len;i++)
			{
      LPS22HB_Read(LPS22HB_REG_ADDR[i],&reg_data,1);
      USART6_printf("REG[%#x]:%#x\r\n",LPS22HB_REG_ADDR[i],reg_data);
			}
}
uint32_t LPS22HB_Press_Data_Get(void)
{
    uint32_t press_data=0;
    LPS22HB_Read(LPS22HB_REG_PRESS_OUT_ADDR,(uint8_t*)&press_data,3);
    return press_data;
}

void bsp_LPS22HB_Init(void)
{
   uint32_t press_data=0;
   
   LPS22HB_Pin_Init();
   bsp_I2C1_Init();
   
  // LPS22HB_Test();
   press_data=LPS22HB_Press_Data_Get();
   USART6_printf("press:%#x\r\n",press_data);

}

void LPS22HB_INT2_IRQHandler(void)
{
	  if(__HAL_GPIO_EXTI_GET_IT(LPS22HB_INT2_PIN) != RESET) 
	  {
		     __HAL_GPIO_EXTI_CLEAR_IT(LPS22HB_INT2_PIN);     
	  }  
}

