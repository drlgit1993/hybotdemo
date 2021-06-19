/**
  * @file    bsp_dwt.h
  * @author  DRL
  * @version V1.0.0
  * @date    10-April-2020
  * @brief   
  *          	
**/


/*
寄存器CYCCNT是一个向上的32位计数器，内核时钟跳动一次，该计数器就加1，当CYCCNT溢出后会清0重新向上计数
精度=1/内核时钟，例如STM32F405的内核时钟为168MHz,那么精度=1/168M=6ns;
最长能记录的时间=(2^32)/内核时钟，如STM32F405的内核时钟为168MHz,那么最长能记录的时间=(2^32)/168M=25s;
*/

/* Includes ------------------------------------------------------------------*/
#include "bsp_dwt.h"



/**
  * @func    :DWT初始化
  * @param   :none
  * @retval  :none
**/
 uint32_t bsp_dwt_init(void)
 {
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;
    DWT_CYCCNT = (uint32_t)0u;
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;

    return((uint32_t)DWT_CYCCNT);
 }
 /*******************************************************************************/

/**
  * @func    :获取DWT计数值
  * @param   :none
  * @retval  :none
**/
uint32_t dwt_get(void)
 {
	 uint32_t get_ts;
	 uint32_t ts;
	 static uint32_t ts_back;
	 
	 get_ts=DWT_CYCCNT;
	 
	 if(get_ts<ts_back)
	 {
		 ts=0xffffffff-ts_back+get_ts;
		 ts=ts+ts_back;
	 }
	 else
	 {
		 ts=get_ts;
	 }
	 ts_back=ts;
	 
   return(ts);
 }
 /**
  * @func    :获取cpu当前时间，单位ms
  * @param   :none
  * @retval  :none
**/
 float get_cpu_time_ms(void)
 {
	 float time=0;
	 
	 time=(float)dwt_get()/168000;
	 
   return time;
 }
 /**
  * @func    :获取cpu当前时间，单位us
  * @param   :none
  * @retval  :none
**/
 float get_cpu_time_us(void)
 {
	 float time=0;
	 
	 time=(float)dwt_get()/168;
	 
   return time;
 }
 /*******************************************************************************/
