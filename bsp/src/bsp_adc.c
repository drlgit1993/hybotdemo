#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;

ADC_HandleTypeDef ADC_Handle;


static void bsp_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    BAT_ADC_GPIO_CLK_ENABLE();
        
    GPIO_InitStructure.Pin = BAT_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; 
    HAL_GPIO_Init(BAT_ADC_GPIO_PORT, &GPIO_InitStructure);		
}

static void bsp_ADC_Mode_Config(void)
{
    ADC_ChannelConfTypeDef ADC_Config;
    BAT_ADC_CLK_ENABLE();
    /* -------------------ADC Init �ṹ�� ���� ��ʼ��------------------------*/

    ADC_Handle.Instance = ADC1;
    // ʱ��Ϊfpclk 4��Ƶ	
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC �ֱ���
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    // ��ֹɨ��ģʽ����ͨ���ɼ�����Ҫ	
    ADC_Handle.Init.ScanConvMode = DISABLE; 
    // ����ת��	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // ������ת��	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // ������ת������
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //��ֹ�ⲿ���ش���    
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //ʹ������������ⲿ�����������ã�ע�͵�����
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //�����Ҷ���	
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //ת��ͨ�� 1��
    ADC_Handle.Init.NbrOfConversion = 1;
    //ʹ������ת������
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //ת����ɱ�־
    ADC_Handle.Init.EOCSelection          = DISABLE;    
    // ��ʼ��ADC	                          
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = BAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // ����ʱ����	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    ADC_Config.Offset       = 0;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

    HAL_ADC_Start_IT(&ADC_Handle);
}

void bsp_ADC_Init(void)
{
	  bsp_ADC_GPIO_Config();
	  bsp_ADC_Mode_Config();
   HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
   HAL_NVIC_EnableIRQ(ADC_IRQn);
}

/**
  * @brief  ת������жϻص�������������ģʽ��
  * @param  AdcHandle : ADC���
  * @retval ��
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* ��ȡ��� */
  ADC_ConvertedValue = HAL_ADC_GetValue(AdcHandle);
}

void ADC_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&ADC_Handle);
}
