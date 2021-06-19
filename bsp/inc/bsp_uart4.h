#ifndef _BSP_UART4_H
#define _BSP_UART4_H

/* Includes ------------------------------------------------------------------*/
#include "bsp_usart.h"
#include "bsp_usart_def.h"
#include "ESP32_task.h"

#ifdef __cplusplus
 extern "C" {
#endif



#define USE_UART4_TX
#define USE_UART4_RX

#if defined(USE_UART4_TX) ||defined(USE_UART4_RX) 
  #define UART4_CLK_ENABLE()                __HAL_RCC_UART4_CLK_ENABLE()
  #define UART4_AF                      	   GPIO_AF8_UART4
  #define UART4_BAUDRATE										          (115200)
  #define UART4_MAINPRI										           4									/*PreemptionPriority*/
  #define UART4_SUBPRI										            0									/*SubPriority*/
#endif


#ifdef USE_UART4_RX
  #define UART4_RX_MAX                      (1024)
 /*USART1-RX:PA10*/
  #define UART4_RX_GPIO_PORT                GPIOA
  #define UART4_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define UART4_RX_PIN                      GPIO_PIN_1

  #define UART4_RX_DMA_STREAM							        DMA1_Stream2
  #define UART4_RX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define UART4_RX_DMA_IRQn								         DMA1_Stream2_IRQn
  #define UART4_RX_DMA_IRQHandler           DMA1_Stream2_IRQHandler
#endif

#ifdef USE_UART4_TX
  /*USART1-TX:PA9*/
  #define UART4_TX_GPIO_PORT                GPIOA
  #define UART4_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define UART4_TX_PIN                      GPIO_PIN_0


  #define UART4_TX_DMA_STREAM							        DMA1_Stream4
  #define UART4_TX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define UART4_TX_DMA_IRQn									        DMA1_Stream4_IRQn
  #define UART4_TX_DMA_IRQHandler							    DMA1_Stream4_IRQHandler
#endif
/***********************************************************************/



void bsp_UART4_init(void);
void bsp_uart4_gpio_Deinit(void);
Que_t *Que_uart4_RX_get(void);
void UART4_RxCpltCallback(void);
void UART4_IdleCpltCallback(void);

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void UART4_data_send(uint8_t *pData, uint16_t Size);


/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void UART4_data_DMA_send(uint8_t *pData, uint16_t Size);

void ESP32_Send_Cmd(void *pstr);
void UART4_Cnt_Get(void);
uint8_t Is_UART4_Received(void);
void UART4_Cnt_Flush(void);
uint16_t UART4_Data_EnQueue(Que_t *que);
uint16_t UART4_Data_Copy(uint8_t *pbuf);
void UART4_Data_Printf(void);


#ifdef __cplusplus
}
#endif

#endif
