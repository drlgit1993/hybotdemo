#ifndef _BSP_USART2_H
#define _BSP_USART2_H

/* Includes ------------------------------------------------------------------*/
#include "bsp_usart.h"
#include "bsp_usart_def.h"
#include "air724_task.h"


#ifdef __cplusplus
 extern "C" {
#endif



#define USE_USART2_TX
#define USE_USART2_RX

#if defined(USE_USART2_TX) ||defined(USE_USART2_RX) 
  #define USART2_CLK_ENABLE()                __HAL_RCC_USART2_CLK_ENABLE()
  #define USART2_AF                      	   GPIO_AF7_USART2
  #define USART2_BAUDRATE										          (115200)
  #define USART2_MAINPRI										           1									/*PreemptionPriority*/
  #define USART2_SUBPRI										            1									/*SubPriority*/
  #define USART2_TX_MAX                      (512)
#endif


#ifdef USE_USART2_RX

  #define USART2_RX_MAX                     (1024)

  #define USART2_RX_GPIO_PORT                GPIOA
  #define USART2_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define USART2_RX_PIN                      GPIO_PIN_3

  #define USART2_RX_DMA_STREAM							        DMA1_Stream5
  #define USART2_RX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define USART2_RX_DMA_IRQn								         DMA1_Stream5_IRQn
  #define USART2_RX_DMA_IRQHandler           DMA1_Stream5_IRQHandler
#endif

#ifdef USE_USART2_TX
  #define USART2_TX_GPIO_PORT                GPIOA
  #define USART2_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
  #define USART2_TX_PIN                      GPIO_PIN_2


  #define USART2_TX_DMA_STREAM							        DMA1_Stream6
  #define USART2_TX_DMA_CHANNEL							       DMA_CHANNEL_4
  #define USART2_TX_DMA_IRQn									        DMA1_Stream6_IRQn
  #define USART2_TX_DMA_IRQHandler							    DMA1_Stream6_IRQHandler
#endif
/***********************************************************************/



void bsp_USART2_init(void);
void bsp_usart2_gpio_Deinit(void);
Que_t *Que_usart2_RX_get(void);
void USART2_RxCpltCallback(void);
void USART2_IdleCpltCallback(void);

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART2_data_send(uint8_t *pData, uint16_t Size);


/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART2_data_DMA_send(uint8_t *pData, uint16_t Size);

/**
* @brief  EC20发送字符串
* @param  str：待发送的数据
* @retval none
*/
void EC20_Send_cmd(void *pstr);

void  bsp_USART2_DeInit(void);

void  bsp_USART2_DeInit(void);

void USART2_Data_Receive(Que_t *que);

uint16_t USART2_Cnt_Len(void);
void USART2_Cnt_Get(void);
uint8_t Is_USART2_Received(void);
void USART2_Cnt_Flush(void);
uint16_t USART2_Data_EnQueue(Que_t *que);
uint16_t USART2_Data_Copy(uint8_t *pbuf);
void USART2_Data_Printf(void);

#ifdef __cplusplus
}
#endif

#endif
