/**
  * @file    bsp_EC20.h
  * @author  DRL
  * @version V1.0.0
  * @date    3-April-2020
  * @brief   
  *          	
**/
#ifndef  _BSP_EC20_H
#define	 _BSP_EC20_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"

#include "bsp_usart.h"
#include "bsp_led.h"



#define EC20_PWR_PORT								          GPIOB
#define EC20_PWR_PIN									          GPIO_PIN_2
#define	EC20_PWR_CLK_ENABLE()									 __HAL_RCC_GPIOB_CLK_ENABLE()

#define	EC20_RST_PORT								          GPIOB
#define	EC20_RST_PIN									          GPIO_PIN_1
#define	EC20_RST_CLK_ENABLE()									 __HAL_RCC_GPIOB_CLK_ENABLE()

#define	EC20_RI_PORT									          GPIOB
#define	EC20_RI_PIN										          GPIO_PIN_14
#define	EC20_RI_CLK_ENABLE()										 __HAL_RCC_GPIOB_CLK_ENABLE()

#define	EC20_DTR_PORT									         GPIOB
#define	EC20_DTR_PIN									          GPIO_PIN_15
#define	EC20_DTR_CLK_ENABLE()									 __HAL_RCC_GPIOB_CLK_ENABLE()

#define	EC20_WAKEUP_PORT							        GPIOA
#define	EC20_WAKEUP_PIN								        GPIO_PIN_7
#define	EC20_WAKEUP_CLK_ENABLE()						 __HAL_RCC_GPIOA_CLK_ENABLE()

#define	EC20_AIRPLANE_PORT						       GPIOA
#define	EC20_AIRPLANE_PIN							       GPIO_PIN_6
#define	EC20_AIRPLANE_CLK_ENABLE()					__HAL_RCC_GPIOA_CLK_ENABLE()

#define	EC20_READY_PORT								        GPIOB
#define	EC20_READY_PIN								         GPIO_PIN_0
#define	EC20_READY_CLK_ENABLE()								__HAL_RCC_GPIOB_CLK_ENABLE()

#define	EC20_PWR_EN_PORT							        GPIOA
#define	EC20_PWR_EN_PIN								        GPIO_PIN_8
#define	EC20_PWR_EN_CLK_ENABLE()							__HAL_RCC_GPIOA_CLK_ENABLE()

#define EC20_PWR_LOW()										          (EC20_PWR_PORT->BSRR =EC20_PWR_PIN)
#define EC20_PWR_HIGH()									          (EC20_PWR_PORT->BSRR =(uint32_t)EC20_PWR_PIN << 16)


#define EC20_REST_ON()									          (EC20_REST_PORT->BSRR = EC20_REST_PIN)
#define EC20_WAKEUP_ON()								         (EC20_WAKEUP_PORT->BSRR = EC20_WAKEUP_PIN )
#define EC20_AIRPLANE_ON()							        (EC20_AIRPLANE_PORT->BSRR = EC20_AIRPLANE_PIN)


#define	EC20_PWR_EN_ON()								         (EC20_PWR_EN_PORT->BSRR =EC20_PWR_EN_PIN)
#define	EC20_PWR_EN_OFF()								        (EC20_PWR_EN_PORT->BSRR =(uint32_t)EC20_PWR_EN_PIN << 16)




#define EC20_USE_RI										          (0)
#define EC20_USE_DTR									          (0)
#define	EC20_USE_AIRPLANE							       (0)
#define	EC20_USE_READY								         (0)

/**
* @brief  EC20模块硬件初始化
* @param  none
* @retval none
*/
void bsp_EC20_Hardware_init(void);


#ifdef __cplusplus
}
#endif

#endif	/*_BSP_EC20_H*/
