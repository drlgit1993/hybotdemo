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
* @brief  发送一个字节的数据
* @param  pUSARTx：待发送的串口号：如USART1,USART2...；Data：待发送的数据
* @retval none
*/
void UART_SendByte( UART_HandleTypeDef *huart, uint8_t Data );

/**
* @brief  发送指定长度的数据
* @param  pUSARTx：待发送的串口号：如USART1,USART2...；str：待发送的数据；strlen:需要发送的数据长度
* @retval none
*/
void UART_SendStr_length( UART_HandleTypeDef *huart, uint8_t *str,uint32_t strlen );

/**
* @brief  发送字符串
* @param  pUSARTx：待发送的串口号：如USART1,USART2...；str：待发送的字符串
* @retval none
*/
void UART_SendStr( UART_HandleTypeDef *huart, uint8_t *str);


#ifdef __cplusplus
}
#endif

#endif

