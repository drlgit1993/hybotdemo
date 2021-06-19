
/**
  * @file    bsp_timer.c
  * @author  DRL
  * @version V1.0.0
  * @date    20-April-2020
  * @brief   
  *          	
**/

/*
 * ע�⣺TIM_HandleTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 
 ��ʱʱ��=TIM_Period*(TIM_Prescaler/TIMxCLK),TIMxCLK = 2 * PCLK1 ,PCLK1 = HCLK / 4 => TIMxCLK=HCLK/2=SystemCoreClock/2
 */


#include "bsp_timer.h"
#include "bsp_usart6.h"
static TIM_HandleTypeDef htim3;

uint32_t led_cnt=0;
static TIM_HandleTypeDef htim6;

#ifdef USE_TIMER7
static TIM_HandleTypeDef htim7;
volatile unsigned long long FreeRTOS_RunTime_Ticks;
#endif


#ifdef USE_TIMER7
/**
  * @brief  ��ʼ����ʱ��7
  * @param  ��
  * @retval ��
��ʱ����100mS,10HZ
 */
static void bsp_timer3_init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
   TIM_MasterConfigTypeDef sMasterConfig = {0};

			htim3.Instance = TIM3;
			htim3.Init.Prescaler = (8400-1);
			htim3.Init.Period =  (1000-1);
   htim3.Init.CounterMode=TIM_COUNTERMODE_UP;
   htim3.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
			if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
			{
					Error_Handler();
			}
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
			{
					Error_Handler();
			}
			sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
			{
					Error_Handler();
			}
		/* Enable the TIM Update interrupt */
			__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);

			__HAL_TIM_ENABLE(&htim3);
}

/**
  * @brief  ��ʼ����ʱ��7
  * @param  ��
  * @retval ��
��ʱ����50us��Ƶ��20KHZ
 */
static void bsp_timer7_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = (84-1);
  htim7.Init.Period =  (50-1);
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim7, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
 /* Enable the TIM Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&htim7);
}
#endif

/**
  * @brief  ��ʼ����ʱ��6��5ms����һ���ж�
  * @param  ��
  * @retval ��
 */
static void bsp_timer6_init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = (8400-1);
  htim6.Init.Period =  (50-1);
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
 /* Enable the TIM Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&htim6);
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
 if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }

  if(tim_baseHandle->Instance==TIM6)
  {
    __HAL_RCC_TIM6_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
  }

#ifdef USE_TIMER7
if(tim_baseHandle->Instance==TIM7)
  {
    __HAL_RCC_TIM7_CLK_ENABLE();
       HAL_NVIC_SetPriority(TIM7_IRQn, 2, 0);
       HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
#endif
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
 if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }

  if(tim_baseHandle->Instance==TIM6)
  {
    __HAL_RCC_TIM6_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
  }

#ifdef USE_TIMER7
  if(tim_baseHandle->Instance==TIM7)
  {
    __HAL_RCC_TIM7_CLK_DISABLE();
HAL_NVIC_DisableIRQ(TIM7_IRQn);
  }
#endif
}
/**
  * @brief  ��ʼ��������ʱ����ʱ
  * @param  ��
  * @retval ��
 */
void bsp_TIMx_init(void)
{
   bsp_timer3_init();
   bsp_timer6_init();
   bsp_timer7_init();
}

void TIM3_IRQHandler(void)
{
HAL_TIM_IRQHandler(&htim3);
}

void TIM7_IRQHandler(void)
{
HAL_TIM_IRQHandler(&htim7);
}

void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if(htim->Instance==TIM3)
	  { 

   }

   if(htim->Instance==TIM6)
	  {   
       GNSS_Receive_Date();
       led_cnt++;

							if((led_cnt % 100)==0)
							{
											LED_SYS_TOGGLE();
							}
	  }

			if(htim->Instance==TIM7)
			{
      FreeRTOS_RunTime_Ticks++;
			}
}


