#ifndef _LSM6DSL_TASK_H
#define _LSM6DSL_TASK_H

#include "lsm6dsl_reg.h"
#include "bsp_spi.h"
#include "bsp_usart6.h"

#ifdef __cplusplus
extern "C"{
#endif

#define LSM6DSL_INT1_PORT				                GPIOA
#define LSM6DSL_INT1_PIN				                 GPIO_PIN_4
#define LSM6DSL_INT1_CLK_ENABLE()				        __HAL_RCC_GPIOA_CLK_ENABLE()
#define LSM6DSL_INT1_EXTI_IRQ                EXTI4_IRQn
#define LSM6DSL_INT1_IRQHandler              EXTI4_IRQHandler


void lsm6dsl_Task_Create(void);


#ifdef __cplusplus
}
#endif

#endif
