/**
  * @file    bsp_dwt.h
  * @author  DRL
  * @version V1.0.0
  * @date    10-April-2020
  * @brief   
  *          	
**/


/*
�Ĵ���CYCCNT��һ�����ϵ�32λ���������ں�ʱ������һ�Σ��ü������ͼ�1����CYCCNT��������0�������ϼ���
����=1/�ں�ʱ�ӣ�����STM32F405���ں�ʱ��Ϊ168MHz,��ô����=1/168M=6ns;
��ܼ�¼��ʱ��=(2^32)/�ں�ʱ�ӣ���STM32F405���ں�ʱ��Ϊ168MHz,��ô��ܼ�¼��ʱ��=(2^32)/168M=25s;
*/

/* Includes ------------------------------------------------------------------*/
#include "bsp_dwt.h"



/**
  * @func    :DWT��ʼ��
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
  * @func    :��ȡDWT����ֵ
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
  * @func    :��ȡcpu��ǰʱ�䣬��λms
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
  * @func    :��ȡcpu��ǰʱ�䣬��λus
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
