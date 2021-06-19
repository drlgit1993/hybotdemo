/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
#include "bsp_spi.h"

/* USER CODE BEGIN 0 */
 
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{
  __HAL_SPI_DISABLE(&hspi1);
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_SPI_ENABLE(&hspi1);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA7     ------> SPI1_MOSI
    PA6     ------> SPI1_MISO
    PA5     ------> SPI1_SCK
    PC4     ------> SPI1_NSS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		  LSM6DSL_CS_DISABLE();

  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA7     ------> SPI1_MOSI
    PA6     ------> SPI1_MISO
    PA5     ------> SPI1_SCK
    PC4     ------> SPI1_NSS
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int32_t bsp_spi_read(void *handle, uint8_t reg_addr,  uint8_t *pRxData, uint16_t len)
{
			HAL_StatusTypeDef rslt=HAL_OK;

			uint8_t reg=reg_addr;
			LSM6DSL_CS_ENABLE();
			HAL_Delay(5);
			rslt=HAL_SPI_TransmitReceive(&hspi1,&reg,pRxData,len,1000);
			HAL_Delay(5);
			LSM6DSL_CS_DISABLE();

  return ((int8_t)rslt);
}

int32_t bsp_spi_write(void *handle, uint8_t reg_addr, const uint8_t *pTxData, uint16_t len)
{
   HAL_StatusTypeDef rslt=HAL_OK;
   uint8_t RxData=0;
   
   (void)reg_addr;

   LSM6DSL_CS_ENABLE();
   HAL_Delay(5);
	  rslt=HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)pTxData,&RxData,len,1000);
   HAL_Delay(5);
	  LSM6DSL_CS_DISABLE();

   return ((int8_t)rslt);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
