#ifndef NTRIP_TEST__H
#define NTRIP_TEST__H
#ifdef __cplusplus
 extern "C" {
#endif

#include "bsp_i2c.h"
#include "tool.h"

#define NTRIP_LOG_EN 1

#if NTRIP_LOG_EN==1

#define  NTRIP_LOG(...) \
{ \
  printf(__VA_ARGS__); \
  printf("\r\n"); \
}
#else
#define  NTRIP_LOG(...) 
#endif



void ntrip_para_load(void);

#ifdef __cplusplus
}
#endif
#endif 
