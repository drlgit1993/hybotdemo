#ifndef _IAP_H
#define _IAP_H

#include "bsp_usart6.h"
#include "bsp_flash.h"
#include "syscfg.h"

#ifdef __cplusplus
extern "C"{
#endif

void Iap_Handle(void *pbuf,uint16_t len);


#ifdef __cplusplus
}
#endif

#endif
