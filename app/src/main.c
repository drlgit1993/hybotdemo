/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_usart.h"
#include "bsp_timer.h"
#include "bsp_dwt.h"
#include "bsp_iwdg.h"
#include "bsp_i2c.h"
#include "syscfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "app_mem.h"
#include "disk_task.h"
#include "cm_backtrace.h"
#include "time_task.h"
#include "bsp_sdio_test.h"
#include "esp32_task.h"
#include "gnss_task.h"
#include "lps22hb.h"
#include "ak09918.h"
#include "lsm6dsl_task.h"
#include "bsp_power.h"
#include "bsp_led.h"
#include "bsp_bmi160.h"
static void SystemClock_Config(void);
static void flash_OB_set(void);
void Reset_Source_chk(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
   HAL_Init();
   SystemClock_Config();
   bsp_GPIO_Init();
//   flash_OB_set();
   Sys_Power_Init();
   
   USART6_Task_Create();
   bsp_Led_Init();
   App_Mem_Init(); 
   HYBOT_VERSION_OUT(); 
   bsp_TIMx_init();
   cm_backtrace_init("hybotdemo_V100","1.0","1.0");
   Disk_Task_Create(); 

   bsp_LPS22HB_Init();
   lsm6dsl_Task_Create();
  // bsp_Ak09918_init();

   GNSS_Task_Create();
  // AIR724_Task_Create();
  // ESP32_Task_Create();

   Timer_Task_Create();
 //  bsp_IWDG_Init();
   vTaskStartScheduler();

   return 0;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
  RCC_OscInitStruct.LSEState=RCC_LSE_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;

  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 24;

  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }


}

/* USER CODE BEGIN 4 */
static void flash_OB_set(void)
{
//  FLASH_OBProgramInitTypeDef pOBInit;

//  HAL_FLASHEx_OBGetConfig(&pOBInit);

//	if((OB_RDP_LEVEL_0 == pOBInit.RDPLevel)||(OB_BOR_LEVEL3 != pOBInit.BORLevel))
//	{
//			HAL_FLASH_Unlock();
//			HAL_FLASH_OB_Unlock();

			/*Set the read protection level.*/
		/*	if(OB_RDP_LEVEL_0 == pOBInit.RDPLevel)
			{
					pOBInit.OptionType=OPTIONBYTE_RDP;
					pOBInit.RDPLevel=OB_RDP_LEVEL_1;
					HAL_FLASHEx_OBProgram(&pOBInit);
			}
*/
			/*Set the BOR Level.*/
		/*	if(OB_BOR_LEVEL3 != pOBInit.BORLevel)
			{
				pOBInit.OptionType=OPTIONBYTE_BOR;
				pOBInit.BORLevel=OB_BOR_LEVEL3;
				HAL_FLASHEx_OBProgram(&pOBInit);
			}
*/
//			HAL_FLASH_OB_Launch();
//			HAL_FLASH_OB_Lock();
//			HAL_FLASH_Lock();
//			}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
printf("error occure\r\n");
  /* USER CODE END Error_Handler_Debug */
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
