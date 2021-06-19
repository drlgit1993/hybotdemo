#include "utils.h"
#include <string.h>
#include <ctype.h>


static uint32_t wTick;

/******************************************************************************
* @brief This function is called to increment  a global variable "uwTick"
*        used as application time base.
* @note In the default implementation, this variable is incremented each 1ms
*       in Systick ISR.
* @note This function is declared as __weak to be overwritten in case of other 
*      implementations in user file.
* @retval None
****************************************************************************/
void HAL_IncTick(void)
{
  wTick++;
}

/**********************************************************************************
* @brief Provides a tick value in millisecond.
* @note   This function is declared as __weak  to be overwritten  in case of other 
*       implementations in user file.
* @retval tick value
********************************************************************************/
uint32_t HAL_GetTick(void)
{
  return wTick;
}

/*********************************************************************************
* @brief This function provides accurate delay (in milliseconds) based 
*        on variable incremented.
* @note In the default implementation , SysTick timer is the source of time base.
*       It is used to generate interrupts at regular time intervals where uwTick
*       is incremented.
* @note ThiS function is declared as __weak to be overwritten in case of other
*       implementations in user file.
* @param Delay: specifies the delay time length, in milliseconds.
* @retval None
*******************************************************************************/
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}


/********************************************************************************
* @brief        : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
* @param pbDest : output buffer
* @param pbDest : input buffer
* @param nLen   : string length
* return value  : void     
******************************************************************************/
void StrToHex(uint8_t *pbDest, uint8_t *pbSrc, uint8_t nLen)
{
  uint8_t h1,h2;
  uint8_t s1,s2;
  uint8_t i;
  
  for (i=0; i< nLen; i++)
  {
    h1 = pbSrc[2 * i];
    h2 = pbSrc[2 * i + 1];
    
    s1 = toupper(h1) - 0x30;
    if (s1 > 9) 
      s1 -= 7;
    
    s2 = toupper(h2) - 0x30;
    if (s2 > 9) 
      s2 -= 7;
    
    pbDest[i] = s1 * 16 + s2;
  }
}

/*********************************************************************
* @brief            : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
* @param pbDest     : for converted ascii string 
* @param pbDest     : for source hex string
*@param nLen        : hex string length 
* @return value     : void
*********************************************************************/
void HexToStr(uint8_t *pbDest, uint8_t *pbSrc, uint8_t nLen)
{
  uint8_t	ddl,ddh;
  uint8_t    i;
  
  for (i=0; i< nLen; i++)
  {
    ddh = 48 + pbSrc[i] / 16;
    ddl = 48 + pbSrc[i] % 16;
    
    if (ddh > 57) 
      ddh = ddh + 7;
    if (ddl > 57) 
      ddl = ddl + 7;
    
    pbDest[i * 2] = ddh;
    pbDest[i * 2 +1] = ddl;
  }
  pbDest[i*2] = '\0';
}

void HexToStrEx(uint8_t *pbDest, uint8_t *pbSrc, uint8_t spliter, uint8_t nLen)
{
  uint8_t	ddl,ddh;
  uint8_t    i;
  
  for (i=0; i< nLen; i++)
  {
    ddh = 48 + pbSrc[i] / 16;
    ddl = 48 + pbSrc[i] % 16;
    
    if (ddh > 57) 
      ddh = ddh + 7;
    if (ddl > 57) 
      ddl = ddl + 7;
    
    pbDest[i * 3] = ddh;
    pbDest[i * 3 + 1] = ddl;
    pbDest[i * 3 + 2] = spliter;
  }
  pbDest[i*3] = '\0';
}

/******************************************************************** 
* @brief          : Get CPU ID .
* @param id       : buffer for store identify.
* @return value   : void
********************************************************************/
void GetLockCode(uint8_t *id)
{
  uint32_t cpuId[3];
  
  cpuId[0]=*(vu32*)(0x1fff7a10);
  cpuId[1]=*(vu32*)(0x1fff7a14);
  cpuId[2]=*(vu32*)(0x1fff7a18);
  
  HexToStr(id, (uint8_t *)cpuId, 12);
}

/*****************************************************************
* @brief          : used for delay. 
* @param mS       : mS numbers for delay.
* @return value   : void
******************************************************************/
void DelyamS(uint32_t mS)
{
  uint32_t end  = HAL_GetTick() + mS;
  while (HAL_GetTick() < end){}
}


/****************************************************************
* @brief          : Init Timer.
* @param timer    : timer object
*
*****************************************************************/
void TimerInit(Timer* timer)
{
  timer->endtime = 0;
}

/***************************************************************
* @brief          : Get Timer State.
* @return value   : 1 for timer Expired and 0 for not
****************************************************************/
uint8_t TimerIsExpired(Timer* timer)
{
  return (TimerLeftMS(timer) == 0) ? 1 : 0;
}

/***************************************************************
* @brief          : Get Timer End Time by timeout_ms and current 
*                   time.
* @return value   : 1 for timer Expired and 0 for not
****************************************************************/
void TimerCountdownMS(Timer* timer, uint16_t timeout_ms)
{
  timer->endtime = HAL_GetTick();
  timer->endtime += timeout_ms;
}

/***************************************************************
* @brief          : Recore End Time Uinit as S, timeout unit S 
*                   time.
* @return value   : 1 for timer Expired and 0 for not
****************************************************************/
void TimerCountdown(Timer* timer, uint16_t timeout) 
{
  TimerCountdownMS(timer, timeout * 1000);
}

/************************************************************** 
* @brief            : Get left timer uint as ms
* @return value     : left time unit as mS.
***************************************************************/
uint32_t TimerLeftMS(Timer* timer) 
{
  uint32_t now = HAL_GetTick();
  
  return (timer->endtime < now) ? 0 : (timer->endtime - now);
}

/************************************************************** 
* @brief          : convert string to lower
* @param pstr     : store source and result string
* @return value   : void
**************************************************************/
void strlwr(uint8_t *pstr)
{
  uint16_t i = 0;
  
  uint16_t len = strlen((char *)pstr);
  
  for (i = 0; i < len;i++)
  {
    pstr[i] = tolower(pstr[i]);
  }
}


/*********************************************************************
* @brief            : delete space pre and after the string
* @param strIn      : buffer for source string 
* @param strOut     : for source hex string
*@param nLen        : buffer for destination string
* @return value     : void
*********************************************************************/
void trim(char *strIn, char *strOut){
  
  uint16_t i = 0, j;
  
  j = strlen(strIn) - 1;
  
  while(strIn[i] == ' ')
    ++i;
  
  while(strIn[j] == ' ')
    --j;
  
  strncpy(strOut, strIn + i , j - i + 1);
  strOut[j - i + 1] = '\0';
}


/*********************************************************************
* @brief            : slide filter
* @param buf        : in and out for the data to be filtered
* @return value     : void
*********************************************************************/
void Slide_filter(uint16_t buf[], uint16_t len)
{
  uint16_t i = 0, j = 0, sum = 0;
  for (i = 0; i < len; i++)
  {
    sum = 0;
    for (j = 0; j <= i;j++)
    {
      sum += buf[j];
    }
    buf[i] = sum / j;
  }
}
/*********************************************************************
* @brief            : kalman filter.
* @param inData     : data to be filter     
* @return value     : result
*********************************************************************/
float kalmanFilter(float inData) 
{
  
  p = p + q; 
  kGain = p / (p+r);
  
  inData = prevData + (kGain * (inData - prevData)); 
  p = (1 - kGain) * p;
  
  prevData = inData;
  
  return inData; 
}

/*********************************************************************
* @brief            : Create a 3 demensions array
* @param m          : x demension    
* @param n          : y demension   
* @param t          : z demension   
* @return value     : pointer for the array created.
*                   Memory size = m * 4 + m * n * 4 + m * n * t * sizeof(elment type)
*********************************************************************/
uint16_t *** New3DArray(uint16_t m, uint16_t n, uint16_t t)
{
  uint16_t  i = 0;
  uint16_t  k = 0;
  uint16_t  *** result = NULL; 
  
  /*Check parameters*/
  if((m > 0) && (n > 0) && (t > 0))
  {
    uint16_t    **pp = NULL;
    uint16_t    *p = NULL;
    
    /*Get m pointer variables to point 2 level pointer*/
    result = (uint16_t***)malloc(m * sizeof(uint16_t **)); 
    
    /*get m x n pointer variables to pointer 1 level pointer*/
    pp = (uint16_t**)malloc(m * n * sizeof(uint16_t*)); 
    
    /*get m x n x t element*/
    p = (uint16_t*)malloc(m * n * t * sizeof(uint16_t));
    
    if((result != NULL) && (pp != NULL) && (p != NULL))
    {
      for(i = 0;i < m;i++)
      {
        /*init 3 level pointer*/
        result[i] = pp + i * n;
        
        for (k = 0;k < n;k++)
        {
          result[i][k] = p + k * t;
        }
        
        p = p + n * t;
      }
    }
    else
    {
      if (result != NULL) free(result);
      if (pp     != NULL) free(pp);
      if (p      != NULL) free(p);
      
      result    = NULL;
      pp        = NULL;
      p         = NULL;
    }
  }
  
  return result;
}


void Delete3D(uint16_t ***p)
{
  if(*p != NULL)
  {
    if(**p != NULL)
    {
      free(**p);
      **p = NULL;
    }
    
    free(*p);
    
    *p = NULL;
  }
  
  free(p);
  p = NULL;
}

uint16_t **New2DArray(uint16_t m, uint16_t n)
{
  uint16_t i = 0;
  uint16_t **result = NULL; 
  
  if((m > 0) && (n > 0))
  {
    uint16_t* p = NULL;
    result = (uint16_t**)malloc(m * sizeof(uint16_t*)); 
    p = (uint16_t*)malloc(m * n * sizeof(uint16_t));
    if((result != NULL) && (p != NULL))
    {
      for (i = 0; i < m;i++)
      {
        result[i] = p + i * n;
      }
    }
    else
    {
      free(result);
      free(p);
      result = NULL;
      p = NULL;
    }
  }
  
  return result;
}
