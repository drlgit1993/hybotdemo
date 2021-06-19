#ifndef _BSP_USART6_H
#define _BSP_USART6_H
#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_usart.h"
#include "bsp_usart_def.h"


#define USE_USART6_TX
#define USE_USART6_RX

//#define USE_USART6_TX_DMA

#if defined(USE_USART6_TX) ||defined(USE_USART6_RX) 
  #define USART6_CLK_ENABLE()                __HAL_RCC_USART6_CLK_ENABLE()
  #define USART6_AF                      	   GPIO_AF8_USART6
  #define USART6_BAUDRATE										          (115200)
  #define USART6_MAINPRI										           7									/*PreemptionPriority*/
  #define USART6_SUBPRI										            0									/*SubPriority*/
#endif


#ifdef USE_USART6_RX
  #define USART6_RX_MAX                     (512)
  /*USART6-RX:PC7*/
  #define USART6_RX_GPIO_PORT                GPIOC
  #define USART6_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
  #define USART6_RX_PIN                      GPIO_PIN_7

  #define USART6_RX_DMA_STREAM							        DMA2_Stream1
  #define USART6_RX_DMA_CHANNEL							       DMA_CHANNEL_5
  #define USART6_RX_DMA_IRQn								         DMA2_Stream1_IRQn
  #define USART6_RX_DMA_IRQHandler           DMA2_Stream1_IRQHandler
#endif

#ifdef USE_USART6_TX
  /*USART6-TX:PC6*/
  #define USART6_TX_MAX                     (512)
  #define USART6_TX_GPIO_PORT                GPIOC
  #define USART6_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
  #define USART6_TX_PIN                      GPIO_PIN_6

#ifdef USE_USART6_TX_DMA
  #define USART6_TX_DMA_STREAM							        DMA2_Stream7
  #define USART6_TX_DMA_CHANNEL							       DMA_CHANNEL_5
  #define USART6_TX_DMA_IRQn									        DMA2_Stream7_IRQn
  #define USART6_TX_DMA_IRQHandler							    DMA2_Stream7_IRQHandler
#endif

#endif
/***********************************************************************/



void bsp_USART6_init(void);
void bsp_usart6_gpio_Deinit(void);
void USART6_RxCpltCallback(void);
void USART6_IdleCpltCallback(void);

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART6_data_send(uint8_t *pData, uint16_t Size);

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART6_data_DMA_send(uint8_t *pData, uint16_t Size);

void USART6_printf(char* fmt,...);
void USART6_DMA_printf(char* fmt,...)  ;
void USART6_Log(char* fmt,...)  ;

uint16_t USART6_Data_Receive(uint8_t *dest);

#ifdef __cplusplus
}
#endif

#endif
