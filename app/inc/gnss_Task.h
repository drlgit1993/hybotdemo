#ifndef _GNSS_TASK_H
#define _GNSS_TASK_H

#include "main.h"

#ifdef __cplusplus
extern "C"{
#endif


#define GNSS_RST_PORT				               GPIOB
#define GNSS_RST_PWR_PIN				            GPIO_PIN_0
#define GNSS_RST_CLK_ENABLE()				       __HAL_RCC_GPIOB_CLK_ENABLE()

#define GNSS_RST_ON()				              (GNSS_RST_PORT->BSRR = (uint32_t)GNSS_RST_PWR_PIN << 16)
#define GNSS_RST_OFF()				             (GNSS_RST_PORT->BSRR = GNSS_RST_PWR_PIN)


#define GNSS_PPS_PORT				              GPIOB
#define GNSS_PPS_PWR_PIN				           GPIO_PIN_1
#define GNSS_PPS_CLK_ENABLE()				      __HAL_RCC_GPIOB_CLK_ENABLE()
#define GNSS_PPS_EXTI_IRQ              EXTI2_IRQn
#define GNSS_PPS_IRQHandler            EXTI2_IRQHandler


#define GNSS_EN_PORT				               GPIOB
#define GNSS_EN_PWR_PIN				            GPIO_PIN_2
#define GNSS_EN_CLK_ENABLE()				       __HAL_RCC_GPIOB_CLK_ENABLE()

#define GNSS_EN_ON()				              (GNSS_EN_PORT->BSRR = (uint32_t)GNSS_EN_PWR_PIN << 16)
#define GNSS_EN_OFF()				             (GNSS_EN_PORT->BSRR = GNSS_EN_PWR_PIN)

void GNSS_Task_Create(void);
void GNSS_Receive_Date(void);


#ifdef __cplusplus
}
#endif


#endif 

