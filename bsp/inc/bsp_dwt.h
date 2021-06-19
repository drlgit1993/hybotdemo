/**
  * @file    bsp_dwt.h
  * @author  DRL
  * @version V1.0.0
  * @date    10-April-2020
  * @brief   
  *          	
**/
#ifndef _BSP_DWT_H
#define	_BSP_DWT_H


#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx.h"
/*******************************************************************************/

/*寄存器基地址*/
#define    DWT_CR    		 					 *(uint32_t*)0xE0001000
#define    DWT_CYCCNT   	 				*(uint32_t*)0xE0001004
#define    DEM_CR    		 					 *(uint32_t*)0xE000EDFC

  /*定义使能位*/
#define    DEM_CR_TRCENA    			(1<<24)
#define    DWT_CR_CYCCNTENA    		(1<<0)

/*******************************************************************************/

/**
  * @func    :DWT初始化
  * @param   :none
  * @retval  :none
**/
uint32_t bsp_dwt_init(void);
/*******************************************************************************/

 /**
  * @func    :获取DWT计数值
  * @param   :none
  * @retval  :none
**/
uint32_t dwt_get(void);
 /**
  * @func    :获取cpu当前时间，单位ms
  * @param   :none
  * @retval  :none
**/
float get_cpu_time_ms(void);
 float get_cpu_time_us(void);
#ifdef __cplusplus
}
#endif

#endif	/*_BSP_DWT_H*/
