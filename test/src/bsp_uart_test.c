/**
  * @file    uart_test.c
  * @author  DRL
  * @version V1.0.0
  * @date    15-April-2020
  * @brief   
  *          	
**/
#include "bsp_uart_test.h"


#define USART_TEST_RX_MAX      (512)


#ifdef USART_TEST_RX_MAX
static uint8_t usart_test_buf[USART_TEST_RX_MAX]={0};
#endif


void bsp_USART3_test(void)
{
  Que_t *Que_usart3=Que_usart3_RX_get();
  uint16_t len=GetQueLength(Que_usart3);

	if(len)
	{
		 memset(usart_test_buf,0,USART_TEST_RX_MAX);
   CpyQueData(Que_usart3,usart_test_buf,len);
   DeQueueEx(Que_usart3,len);

   USART3_data_DMA_send(usart_test_buf,len);
	}
}

void bsp_USART1_test(void)
{
  Que_t *Que_usart1=Que_usart1_RX_get();
  uint16_t len=GetQueLength(Que_usart1);

	 if(len)
	 {
		  memset(usart_test_buf,0,USART_TEST_RX_MAX);
    CpyQueData(Que_usart1,usart_test_buf,len);
    DeQueueEx(Que_usart1,len);

    USART3_data_DMA_send(usart_test_buf,len);
	 }
}

void bsp_USART2_test(void)
{
  Que_t *Que_usart2=Que_usart1_RX_get();
  uint16_t len=GetQueLength(Que_usart2);
	 if(len)
	 {
			 memset(usart_test_buf,0,USART_TEST_RX_MAX);
    CpyQueData(Que_usart2,usart_test_buf,len);
    DeQueueEx(Que_usart2,len);
    USART3_data_DMA_send(usart_test_buf,len);
 	}
}
#ifdef USE_UART4_RX
void bsp_UART4_test(void)
{
  Que_t *Que_usart4=Que_uart4_RX_get();
  uint16_t len=GetQueLength(Que_usart4);
	 if(len)
	 {
			 memset(usart_test_buf,0,USART_TEST_RX_MAX);
    CpyQueData(Que_usart4,usart_test_buf,len);
    DeQueueEx(Que_usart4,len);
    UART4_data_DMA_send(usart_test_buf,len);
 	}
}
#endif

void EC20_throughtout_test(void)
{

  Que_t *Que_usart2=Que_usart2_RX_get();
  Que_t *Que_usart3=Que_usart3_RX_get();
  uint16_t len_usart2=GetQueLength(Que_usart2);
  uint16_t len_usart3=GetQueLength(Que_usart3);

	if(len_usart3)
	{

    memset(usart_test_buf,0,USART_TEST_RX_MAX);
    CpyQueData(Que_usart3,usart_test_buf,len_usart3);
    DeQueueEx(Que_usart3,len_usart3);
    USART2_data_DMA_send(usart_test_buf,len_usart3);
	}

	if(len_usart2)
	{

    memset(usart_test_buf,0,USART_TEST_RX_MAX);
    CpyQueData(Que_usart2,usart_test_buf,len_usart2);
    DeQueueEx(Que_usart2,len_usart2);
    USART3_data_DMA_send(usart_test_buf,len_usart2);
	}
}
