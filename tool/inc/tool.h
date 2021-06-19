/**
  * @file    tool.h
  * @author  DRL
  * @version V1.0.0
  * @date    3-April-2020
  * @brief   
  *          	
**/

#ifndef _TOOL_H
#define _TOOL_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdint.h> 
#include <stdio.h> 
#include <string.h>

#include "stm32f4xx.h"
#include "cmsis_armcc.h"

#define WORD_LO( var )  ((uint8_t)(var))                    /*���ֵ�LSB*/
#define WORD_HO( var )  ((uint8_t)((var)>>8))              /*���ֵ�MSB*/

#define WORD_LL( var )     ((uint8_t)((var)&0xFF))              /*�ֵĵ�LSB*/
#define WORD_LM( var )     ((uint8_t)((var >> 8)&0xFF))         /*�ֵĵ�MSB*/
#define WORD_HL( var )     ((uint8_t)((var >> 16)&0xFF))        /*�ֵĸ�LSB*/
#define WORD_HM( var )     ((uint8_t)((var >> 24)&0xFF))        /*�ֵĸ�MSB*/
#define CHAR2SHORT( m,n )    (((uint16_t)m << 8)  + ((uint16_t)n))

#define sscanf_RTOS(str,format, ...) \
        do{ \
               taskENTER_CRITICAL(); \
               vTaskSuspendAll(); \
               sscanf(str,format,__VA_ARGS__); \
               xTaskResumeAll(); \
               taskEXIT_CRITICAL(); \
        }while(0U)

/**
* @brief  将数字字符串转换成数字
* @param  pdata：需要转换的数字字符串,len：需要转换的数字字符串长度
* @retval 转换后的数字
*/
uint32_t numstr_to_num(void *pdata,uint8_t len);
void buff_set(void *pbuff,uint8_t data,uint16_t size);
void buff_copy(void *dest,void *src,uint16_t size);
/**
* @brief  获取字符串str后面字符":"与字符";"之间的字符串
* @param  pbuf：待获取的缓存,str：起始字符串,dest:获取到字符串
* @retval 获取到的字符串长度
*/
uint8_t get_string_num(uint8_t *pbuf,char *str,uint8_t *dest);
	 
uint8_t *string_search(const void *pbuf,const char *string);
void system_reset(void);

void Get_McuidHex(uint8_t  *p_data);
void Get_McuidAscii(uint8_t  *mcuid);
uint16_t Find_LineIndex(uint8_t *line, int limit,uint16_t len_max);
uint8_t *string_find(const char *s_str,const char *e_str,const char *src,uint16_t *len);

void pb_AddNMEACheckSum(char *pBuf);

#ifdef __cplusplus
}
#endif

#endif	/*_TOOL_H*/
