#ifndef _BSP_BMI160_H
#define _BSP_BMI160_H


#include "bmi160.h"
#include "bmi160_defs.h"
#include "bsp_spi.h"


#ifdef __cplusplus
extern "C"{
#endif

#define BMI160_INT1_PORT				                GPIOA
#define BMI160_INT1_PIN				                 GPIO_PIN_4
#define BMI160_INT1_CLK_ENABLE()				        __HAL_RCC_GPIOA_CLK_ENABLE()
#define BMI160_INT1_EXTI_IRQ                EXTI4_IRQn
#define BMI160_INT1_IRQHandler              EXTI4_IRQHandler



int8_t bsp_bmi160_init(void);
void BMI160_INT2_IRQ_Callback(void);

void bsp_bmi160_interrupt_get(void);
int8_t bsp_bmi160_accel_power_config(uint8_t power);
int8_t bsp_bmi160_gyro_power_config(uint8_t power);
int8_t bsp_bmi160_lowpwr_config(void);

void bsp_bmi160_get_power_mode(void);

void  bmi160_get_accel_data(struct bmi160_sensor_data *accel);
void  bmi160_get_gyro_data(struct bmi160_sensor_data *gyro);
void  bmi160_get_accel_gyro_data(struct bmi160_sensor_data *accel,struct bmi160_sensor_data *gyro);
#ifdef __cplusplus
}
#endif



#endif /*_BSP_BMI160_H*/
