#ifndef _BSP_USART1_H
#define _BSP_USART1_H

/* Includes ------------------------------------------------------------------*/
#include "bsp_usart.h"
#include "bsp_usart_def.h"
#include "bsp_timer.h"
#include "bsp_dwt.h"
#include "stdarg.h"	
#include "usart_task.h"


#ifdef __cplusplus
 extern "C" {
#endif



#define USE_USART1_TX
#define USE_USART1_RX

#if defined(USE_USART1_TX) ||defined(USE_USART1_RX) 
  #define USART1_CLK_ENABLE()                 __HAL_RCC_USART1_CLK_ENABLE()
  #define USART1_AF                      	    GPIO_AF7_USART1
  #define USART1_BAUDRATE										           (9600)
  #define USART1_MAINPRI										            2									/*PreemptionPriority*/
  #define USART1_SUBPRI										             0									/*SubPriority*/
  #define USART1_TX_MAX                       (512)
  #define USART1_RX_MAX                       ( 1024)
#endif


#ifdef USE_USART1_RX
  #define LaneTo_RX_MAX                     (USART1_RX_MAX)
 /*USART1-RX:PA10*/
  #define USART1_RX_GPIO_PORT                GPIOA
  #define USART1_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define USART1_RX_PIN                      GPIO_PIN_10

  #define USART1_RX_DMA_STREAM							        DMA2_Stream5
  #define USART1_RX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define USART1_RX_DMA_IRQn								         DMA2_Stream5_IRQn
  #define USART1_RX_DMA_IRQHandler           DMA2_Stream5_IRQHandler
#endif

#ifdef USE_USART1_TX
  /*USART1-TX:PA9*/
  #define USART1_TX_GPIO_PORT                GPIOA
  #define USART1_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define USART1_TX_PIN                      GPIO_PIN_9


  #define USART1_TX_DMA_STREAM							        DMA2_Stream7
  #define USART1_TX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define USART1_TX_DMA_IRQn									        DMA2_Stream7_IRQn
  #define USART1_TX_DMA_IRQHandler							    DMA2_Stream7_IRQHandler
#endif
/***********************************************************************/

typedef struct
{
 uint8_t *buf;
 volatile uint16_t   cur_count;
 volatile uint16_t   old_count;
 volatile uint8_t     Flag;
}UART_RECV_t;

void bsp_USART1_init(void);
void bsp_usart1_gpio_Deinit(void);
Que_t *Que_usart1_RX_get(void);
void USART1_RxCpltCallback(void);
void USART1_RxHalfCpltCallback(void);
void USART1_IdleCpltCallback(void);
void USART1_printf(char* fmt,...);
/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART1_data_send(uint8_t *pData, uint16_t Size);


/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART1_data_DMA_send(uint8_t *pData, uint16_t Size);

void uart_init(void);


void usart1_data_get(void);
uint8_t Is_USART1_Received(void);
void USART1_Cnt_Get(void);
void USART1_Cnt_Flush(void);
uint16_t USART1_Data_EnQueue(Que_t *que);
void USART1_Data_Printf(void);


#ifdef __cplusplus
}
#endif

#endif
