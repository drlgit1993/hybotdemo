/**
  * @file    crc16.h
  * @author  DRL
  * @version V1.0.0
  * @date    14-April-2020
  * @brief   
  *          	
**/

#ifndef CRC16_H_
#define CRC16_H_

#include <stddef.h>
#include "stm32f4xx.h"


uint16_t crc16_calc(uint8_t *pbuf, uint16_t len);
#endif /* CRC16_H_ */
