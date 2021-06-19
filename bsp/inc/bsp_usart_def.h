/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#ifndef _BSP_USART_DEF_H
#define _BSP_USART_DEF_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */


#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "que.h"



#define USART_TIMEOUT																			(1000)


typedef struct
{
   volatile  uint16_t cur_cnt;
   volatile  uint16_t old_cnt;
}USART_cnt_t;

typedef struct
{
 uint8_t *buff;
 uint8_t flag;
 uint16_t len;
 void(*flag_reset)(uint8_t dev);
}buff_t;

typedef struct
{
	uint16_t current_count;
	uint16_t old_count;
	uint16_t old_pos;
	uint16_t current_pos;
	uint8_t old_memory;
	uint8_t current_memory;
	uint16_t len;
	uint8_t flag;
	uint8_t *buff0;
	uint8_t *buff1;
	void(*flag_reset)(uint8_t dev);
}uart_dma_data_t;



typedef struct
{
  uint8_t  *buf;
  uint8_t  flag;
  uint16_t len;
  uint16_t size;
  void(*flag_reset)(uint8_t dev);
}usart_handler_t,*usart_handler_pt;

typedef struct
{
 usart_handler_t buf;
 uart_dma_data_t dma;
}uart_handle_t;


#ifdef __cplusplus
}
#endif
#endif /*_BSP_USART_DEF_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
