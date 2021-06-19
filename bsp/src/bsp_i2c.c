/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_i2c.h"


/* USER CODE BEGIN 0 */
#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */



I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

void bsp_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 1;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

void bsp_I2C2_Init(void)
{
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 2;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 3;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;

    I2C1_SCL_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = I2C1_SCL_PIN;
    HAL_GPIO_Init(I2C1_SCL_GPIO_PORT, &GPIO_InitStruct);

    I2C1_SCL_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = I2C1_SDA_PIN;
    HAL_GPIO_Init(I2C1_SDA_GPIO_PORT, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  	/* Force the I2C peripheral clock reset */ 
   __HAL_RCC_I2C1_FORCE_RESET();

	 /* Release the I2C peripheral clock reset */ 
   __HAL_RCC_I2C1_RELEASE_RESET();

  /* USER CODE END I2C1_MspInit 1 */
  }

if(i2cHandle->Instance==I2C2)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;

    I2C2_SCL_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = I2C2_SCL_PIN;
    HAL_GPIO_Init(I2C2_SCL_GPIO_PORT, &GPIO_InitStruct);

    I2C2_SDA_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = I2C2_SDA_PIN;
    HAL_GPIO_Init(I2C2_SDA_GPIO_PORT, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  	/* Force the I2C peripheral clock reset */ 
   __HAL_RCC_I2C2_FORCE_RESET();

	 /* Release the I2C peripheral clock reset */ 
   __HAL_RCC_I2C2_RELEASE_RESET();

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
   if(i2cHandle->Instance==I2C1)
   {
       __HAL_RCC_I2C1_CLK_DISABLE();

       HAL_GPIO_DeInit(I2C1_SCL_GPIO_PORT, I2C1_SCL_PIN);

       HAL_GPIO_DeInit(I2C1_SDA_GPIO_PORT, I2C1_SDA_PIN);
   }

   if(i2cHandle->Instance==I2C2)
   {
       __HAL_RCC_I2C2_CLK_DISABLE();

       HAL_GPIO_DeInit(I2C2_SCL_GPIO_PORT, I2C2_SCL_PIN);

       HAL_GPIO_DeInit(I2C2_SDA_GPIO_PORT, I2C2_SDA_PIN);
   }
}

HAL_StatusTypeDef LPS22HB_Write(uint16_t waddr,uint8_t* pbuf, uint16_t wsize)
{
	  	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDRESS, (uint16_t)waddr, I2C_MEMADD_SIZE_8BIT, pbuf, wsize, 100); 

	/* Check the communication status */
	if(status != HAL_OK)
	{
    USART6_printf("I2C1 communicate error!\r\n");
	}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
	{
		
	}

	/* Check if the AK09918 is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&hi2c1, LPS22HB_ADDRESS, I2C_MAX_TRIALS, I2C_MAX_TIMEOUT) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}

HAL_StatusTypeDef LPS22HB_Read(uint16_t raddr,uint8_t* pbuf, uint16_t rsize)
{
	  HAL_StatusTypeDef status = HAL_OK;
	
	  status=HAL_I2C_Mem_Read(&hi2c1,LPS22HB_ADDRESS,raddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pbuf, rsize,1000);

	  return status;
}

HAL_StatusTypeDef AK09918_Write(uint16_t waddr,uint8_t* pbuf, uint16_t wsize)
{
	  	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&hi2c2, AK09918_ADDRESS, (uint16_t)waddr, I2C_MEMADD_SIZE_8BIT, pbuf, wsize, 100); 

	/* Check the communication status */
	if(status != HAL_OK)
	{
    USART6_printf("I2C2 communicate error!\r\n");
	}
	while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
	{
		
	}

	/* Check if the AK09918 is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&hi2c2, AK09918_ADDRESS, I2C_MAX_TRIALS, I2C_MAX_TIMEOUT) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}

HAL_StatusTypeDef AK09918_Read(uint16_t raddr,uint8_t* pbuf, uint16_t rsize)
{
	  HAL_StatusTypeDef status = HAL_OK;
	
	  status=HAL_I2C_Mem_Read(&hi2c2,AK09918_ADDRESS,raddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pbuf, rsize,1000);

	  return status;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
