#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus 
extern "C"{
#endif
	
#include "stm32f4xx.h"
#include "des.h"
#include "crypto.h"
#include "stdlib.h"

/*
Timer strcture for software delay
*/
typedef struct Timer 
{
  uint32_t endtime;
} Timer;

void        HAL_IncTick(void);
uint32_t    HAL_GetTick(void);
void        HAL_Delay(__IO uint32_t Delay);
void        TimerInit(Timer* timer);
void        TimerCountdownMS(Timer* timer, uint16_t timeout_ms);
void        TimerCountdown(Timer* timer,   uint16_t timeout) ;
uint32_t    TimerLeftMS(Timer* timer);
uint8_t     TimerIsExpired(Timer* timer);

void        StrToHex(uint8_t *pbDest, uint8_t *pbSrc, uint8_t nLen);
void        HexToStr(uint8_t *pbDest, uint8_t *pbSrc, uint8_t nLen);
void        GetLockCode(uint8_t *id);
void        DelyamS(uint32_t mS);
void        strlwr(uint8_t *pstr);
void        trim(char *strIn, char *strOut);
void        Slide_filter(uint16_t buf[], uint16_t len);
void        Filtering_7(u16 *pDat, uint16_t num) ;

void        Delay_3CPUT(uint32_t n);
#define     Delay_HnS(HnS) {for(int i=0;i<HnS * 5;i++)__NOP();} 
#define     Delay_nS(nS) Delay_3CPUT(nS * 168 / 2800)

#define     LINE 1024

static      float prevData=0; 
static      float p = 10, q = 0.0001, r = 0.05, kGain=0;

float       kalmanFilter(float inData);

uint16_t    ***New3DArray(uint16_t m, uint16_t n, uint16_t t);
uint16_t    **New2DArray(uint16_t m, uint16_t n);
void        Delete3D(uint16_t ***p);
void        HexToStrEx(uint8_t *pbDest, uint8_t *pbSrc, uint8_t spliter, uint8_t nLen);
uint16_t    DesEncrypto(mbedtls_des_context *des_context, uint8_t desbuffer[], uint8_t srcbuffer[], uint8_t key[], uint16_t size);
uint16_t    DesDecrypto(mbedtls_des_context *des_context, uint8_t desbuffer[], uint8_t srcbuffer[], uint8_t key[], uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
