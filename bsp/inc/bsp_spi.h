/**
  ******************************************************************************
  * File Name          : bsp_spi.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BSP_SPI_H
#define _BSP_SPI_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* USER CODE BEGIN Includes */
#define LSM6DSL_CS_DISABLE()    (GPIOC->BSRR=(uint32_t)GPIO_PIN_4)          /*high level*/
#define LSM6DSL_CS_ENABLE()     (GPIOC->BSRR=((uint32_t)GPIO_PIN_4)<<16)        /*low level*/
/* USER CODE END Includes */

extern  SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

int32_t bsp_spi_read(void *handle, uint8_t reg_addr,  uint8_t *pRxData, uint16_t len);
int32_t bsp_spi_write(void *handle, uint8_t reg_addr,  const uint8_t *pTxData, uint16_t len);
/* USER CODE END Prototypes */



#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
