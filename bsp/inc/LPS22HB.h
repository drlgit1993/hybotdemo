#ifndef _LPS22HB_H
#define _LPS22HB_H

#include "main.h"
#include "bsp_i2c.h"

#ifdef __cplusplus
extern "C"{
#endif

#define LPS22HB_INT2_PORT				                GPIOB
#define LPS22HB_INT2_PIN				                 GPIO_PIN_7
#define LPS22HB_INT2_CLK_ENABLE()				        __HAL_RCC_GPIOB_CLK_ENABLE()
#define LPS22HB_INT2_EXTI_IRQ                EXTI9_5_IRQn
#define LPS22HB_INT2_IRQHandler              EXTI9_5_IRQHandler



/*Interrupt register,R/W,default:00000000,0x00*/
#define LPS22HB_REG_INTERRUPT_CFG            (0x0B)   

/*Pressure threshold registers,R/W,default:00000000,0x00*/
#define LPS22HB_REG_THS_P_L                  (0x0C)   
#define LPS22HB_REG_THS_P_H                  (0x0D)   

/*Who am I,R,default:10110001,0xb1*/
#define LPS22HB_REG_WHO_AM_I                 (0x0F)   

/*Control registers,R/W*/
#define LPS22HB_REG_CTRL_REG1                (0x10)   /*default:00000000,0x00*/
#define LPS22HB_REG_CTRL_REG2                (0x11)   /*default:00010000,0x10*/
#define LPS22HB_REG_CTRL_REG3                (0x12)   /*default:00000000,0x00*/

/*FIFO configuration register,R/W,default:00000000*/
#define LPS22HB_REG_FIFO_CTRL                (0x14) 
  
/*Reference pressure registers,R/W*/
#define LPS22HB_REG_REF_P_XL                 (0x15)   /*default:00000000*/
#define LPS22HB_REG_REF_P_L                  (0x16)   /*default:00000000*/
#define LPS22HB_REG_REF_P_H                  (0x17)   /*default:00000000*/

/*Pressure offset registers,R/W*/
#define LPS22HB_REG_RPDS_L                   (0x18)   /*default:00000000*/
#define LPS22HB_REG_RPDS_H                   (0x19)  /*default:00000000*/

/*Resolution register,R/W,default:00000000*/
#define LPS22HB_REG_RES_CONF                 (0x1A) 

/*Interrupt register,RO*/  
#define LPS22HB_REG_INT_SOURCE               (0x25) 

/*FIFO status register,RO*/  
#define LPS22HB_REG_FIFO_STATUS              (0x26)   

/*Status register,RO*/
#define LPS22HB_REG_STATUS                   (0x27)   

/*Pressure output registers,RO*/
#define LPS22HB_REG_PRESS_OUT_ADDR           (0x28)  
#define LPS22HB_REG_PRESS_OUT_XL             (0x28)   
#define LPS22HB_REG_PRESS_OUT_L              (0x29)   
#define LPS22HB_REG_PRESS_OUT_H              (0x2A)   

/*Temperature output registers,RO*/
#define LPS22HB_REG_TEMP_OUT_L               (0x2B)   
#define LPS22HB_REG_TEMP_OUT_H               (0x2C)   

/*Filter reset register,RO*/
#define LPS22HB_REG_LPFP_RES                 (0x33)  

#define LPS22HB_SPEED_0_HZ                   (0x00)  /*power down mode*/
#define LPS22HB_SPEED_1_HZ                   (0x01)
#define LPS22HB_SPEED_10_HZ                  (0x02)
#define LPS22HB_SPEED_25_HZ                  (0x03)
#define LPS22HB_SPEED_50_HZ                  (0x04)
#define LPS22HB_SPEED_75_HZ                  (0x05)

void LPS22HB_INT_IRQ_Callback(void);
void bsp_LPS22HB_Init(void);

#ifdef __cplusplus
}
#endif


#endif
