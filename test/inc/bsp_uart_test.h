 /**
  * @file    uart_test.h
  * @author  DRL
  * @version V1.0.0
  * @date    15-April-2020
  * @brief   
  *          	
**/
#ifndef _BSP_UART_TEST_H
#define	_BSP_UART_TEST_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
//#include "fml_uart.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_usart3.h"
#include "bsp_uart4.h"


void bsp_USART3_test(void);
void bsp_USART1_test(void);
void bsp_USART2_test(void);
void bsp_UART4_test(void);
void EC20_throughtout_test(void);
#ifdef __cplusplus
}
#endif

#endif	/*_UART_TEST_H*/
