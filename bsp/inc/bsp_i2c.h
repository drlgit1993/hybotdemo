/**
  ******************************************************************************
  * File Name          : bsp_i2c.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BSP_I2C_H
#define _BSP_I2C_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "bsp_usart6.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */

#define I2C_MAX_TRIALS                (100)
#define I2C_MAX_TIMEOUT               (100)

#define AK09918_ADDRESS                   (0x0C<<1)
#define LPS22HB_ADDRESS               (0x5D<<1)

#define I2C1_SDA_GPIO_PORT                GPIOB
#define I2C1_SDA_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SDA_PIN                      GPIO_PIN_9

#define I2C1_SCL_GPIO_PORT                GPIOB
#define I2C1_SCL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C1_SCL_PIN                      GPIO_PIN_8


#define I2C2_SDA_GPIO_PORT                GPIOB
#define I2C2_SDA_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SDA_PIN                      GPIO_PIN_11

#define I2C2_SCL_GPIO_PORT                GPIOB
#define I2C2_SCL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C2_SCL_PIN                      GPIO_PIN_10


/* USER CODE END Private defines */

void bsp_I2C1_Init(void);
void bsp_I2C2_Init(void);


/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef bsp_i2c_Write(uint16_t DevAddress,uint16_t waddr,uint8_t* pbuf, uint16_t Size);
HAL_StatusTypeDef bsp_i2c_read(uint16_t DevAddress,uint16_t raddr,uint8_t* pbuf, uint16_t rsize);

HAL_StatusTypeDef AK09918_Read(uint16_t raddr,uint8_t* pbuf, uint16_t rsize);
HAL_StatusTypeDef AK09918_Write(uint16_t waddr,uint8_t* pbuf, uint16_t wsize);

HAL_StatusTypeDef LPS22HB_Write(uint16_t waddr,uint8_t* pbuf, uint16_t wsize);
HAL_StatusTypeDef LPS22HB_Read(uint16_t raddr,uint8_t* pbuf, uint16_t rsize);


 
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
