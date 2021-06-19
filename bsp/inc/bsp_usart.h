#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "stm32f4xx.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_usart6.h"
#include "bsp_uart4.h"
#include "bsp_usart_def.h"
#include "que.h"

#ifdef __cplusplus
 extern "C" {
#endif





/**
* @brief  ����һ���ֽڵ�����
* @param  pUSARTx�������͵Ĵ��ںţ���USART1,USART2...��Data�������͵�����
* @retval none
*/
void UART_SendByte( UART_HandleTypeDef *huart, uint8_t Data );

/**
* @brief  ����ָ�����ȵ�����
* @param  pUSARTx�������͵Ĵ��ںţ���USART1,USART2...��str�������͵����ݣ�strlen:��Ҫ���͵����ݳ���
* @retval none
*/
void UART_SendStr_length( UART_HandleTypeDef *huart, uint8_t *str,uint32_t strlen );

/**
* @brief  �����ַ���
* @param  pUSARTx�������͵Ĵ��ںţ���USART1,USART2...��str�������͵��ַ���
* @retval none
*/
void UART_SendStr( UART_HandleTypeDef *huart, uint8_t *str);


#ifdef __cplusplus
}
#endif

#endif

