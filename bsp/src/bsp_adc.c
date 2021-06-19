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
    /* -------------------ADC Init 结构体 参数 初始化------------------------*/

    ADC_Handle.Instance = ADC1;
    // 时钟为fpclk 4分频	
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // ADC 分辨率
    ADC_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    // 禁止扫描模式，多通道采集才需要	
    ADC_Handle.Init.ScanConvMode = DISABLE; 
    // 连续转换	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // 非连续转换	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // 非连续转换个数
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //禁止外部边沿触发    
    ADC_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //使用软件触发，外部触发不用配置，注释掉即可
    //ADC_Handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    //数据右对齐	
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //转换通道 1个
    ADC_Handle.Init.NbrOfConversion = 1;
    //使能连续转换请求
    ADC_Handle.Init.DMAContinuousRequests = ENABLE;
    //转换完成标志
    ADC_Handle.Init.EOCSelection          = DISABLE;    
    // 初始化ADC	                          
    HAL_ADC_Init(&ADC_Handle);
    //---------------------------------------------------------------------------
    ADC_Config.Channel      = BAT_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // 采样时间间隔	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    ADC_Config.Offset       = 0;
    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
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
  * @brief  转换完成中断回调函数（非阻塞模式）
  * @param  AdcHandle : ADC句柄
  * @retval 无
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* 获取结果 */
  ADC_ConvertedValue = HAL_ADC_GetValue(AdcHandle);
}

void ADC_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&ADC_Handle);
}
