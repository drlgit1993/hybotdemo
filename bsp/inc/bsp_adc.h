#ifndef _BSP_ADC_H
#define _BSP_ADC_H

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif




#define BAT_ADC_GPIO_PORT              GPIOC
#define BAT_ADC_GPIO_PIN               GPIO_PIN_2
#define BAT_ADC_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()
    
#define BAT_ADC                        ADC1
#define BAT_ADC_CLK_ENABLE()           __ADC1_CLK_ENABLE()
#define BAT_ADC_CHANNEL                ADC_CHANNEL_12



#ifdef __cplusplus
}
#endif


#endif
