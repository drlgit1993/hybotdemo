#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "gnss_task.h"
#include "bsp_led.h"

#define USE_TIMER7

void bsp_TIMx_init(void);


#ifdef __cplusplus
}
#endif

#endif

