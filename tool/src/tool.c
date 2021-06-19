/**
  * @file    tool.c
  * @author  DRL
  * @version V1.0.0
  * @date    3-April-2020
  * @brief   
  *          	
**/
#include "tool.h"

void buff_set(void *pbuff,uint8_t data,uint16_t size)
{
	uint8_t *ptemp=(uint8_t *)pbuff;
	
	while(size--)
	{
		*ptemp ++ =data;
	}
}

void buff_copy(void *dest,void *src,uint16_t size)
{
	uint8_t *pdest=(uint8_t *)dest;
	uint8_t *psrc=(uint8_t *)src;
	
	while(size--)
	{
		*pdest ++ = *psrc ++;		
	}	
}

/**
* @brief  将数字字符串转换成数字
* @param  pdata：需要转换的数字字符串,len：需要转换的数字字符串长度
* @retval 转换后的数字
*/
uint32_t numstr_to_num(void *pdata,uint8_t len)
{
	uint8_t *pstr=(uint8_t *)pdata;
	uint32_t num=0;
	
	while(len)
	{
		num = (num<<3) + (num<<1) + (*pstr - '0');
		
		pstr ++;
		
		len--;
	}
	return num;
}
/**
* @brief  获取字符串str后面字符":"与字符";"之间的字符串
* @param  pbuf：待获取的缓存,str：起始字符串,dest:获取到字符串
* @retval 获取到的字符串长度
*/
uint8_t get_string_num(uint8_t *pbuf,char *str,uint8_t *dest)
{
	char *pret=NULL;
	char *start=NULL;
	char *end=NULL;
	uint8_t str_len=0;
	pret=strstr((const char*)pbuf,(const char*)str);
	if(pret==NULL)
	{
		return 0;
	}
	pret=strstr(pret,":");
	if(pret==NULL)
	{
		return 0;
	}
	start=pret+1;
	
	end=strstr(pret,";");
	
	if(end==NULL)
	{
		return 0;
	}
	
	str_len=end-start;
	
	buff_copy(dest,start,str_len);
	
	return (str_len);
}
uint8_t *string_search(const void *pbuf,const char *string)
{
 return ((uint8_t*)strstr((const char *)pbuf,string));
}

void system_reset(void)
{
  __set_FAULTMASK(1);
  NVIC_SystemReset();
}
/**
  * @func    :get the mcu id
  * @param   :p_data:mcu id buffer
  * @retval  :none
**/
 void Get_McuidHex(uint8_t  *p_data)
{
  uint32_t temp[3];
  uint8_t i=0,j=0;
  temp[0] = *(uint32_t*)(0x1fff7a10);       
  temp[1] = *(uint32_t*)(0x1fff7a14);	   
  temp[2] = *(uint32_t*)(0x1fff7a18);	     

  for(i=0;i<3;i++)
  {
    p_data[j++] = WORD_LL(temp[i]);
    p_data[j++] = WORD_LM(temp[i]);
    p_data[j++] = WORD_HL(temp[i]);
    p_data[j++] = WORD_HM(temp[i]);
  }
}
/**
  * @func    :convert hex to ascii string
  * @param   :p_Dest:ascii string,p_Src:hex srting,Length:hex string length
  * @retval  :none
**/
void HexToStr(uint8_t *p_Dest, uint8_t *p_Src, uint8_t Length)
{
  uint8_t  ddl,ddh;
  uint8_t   i;

  for (i=0; i< Length; i++)
  {
    ddh=48 +(p_Src[i]>>4);
    ddl=48 +(0x0f&p_Src[i]);

    if (ddh > 57)
      ddh = ddh + 7;
    if (ddl > 57)
      ddl = ddl + 7;

    p_Dest[i * 2] = ddh;
    p_Dest[i * 2 +1] = ddl;
  }
  p_Dest[i*2] = '\0';
}

/*******************************************************************************/

/**
  * @func    :get mcu id
  * @param   :mcuid buff
  * @retval  :none
**/
 void Get_McuidAscii(uint8_t  *mcuid)
{
  uint32_t cpuId[3];

  cpuId[0] = *(uint32_t*)(0x1fff7a10);                                        
  cpuId[1] = *(uint32_t*)(0x1fff7a14);		                       
  cpuId[2]= *(uint32_t*)(0x1fff7a18);		                               

  HexToStr(mcuid, (uint8_t *)cpuId, 12);
}
uint16_t Find_LineIndex(uint8_t *line, int limit,uint16_t len_max)
{
   uint16_t idx, counter;
   for (idx = 0, counter = 0; idx < len_max; idx++)
   {
      if ('\0' == line[idx])  
         return 0;
         
      if (',' == line[idx])
      {
         counter++;
         if(counter == limit)
            break;
      }
   }

   return (idx+1);
}
uint8_t *string_find(const char *s_str,const char *e_str,const char *src,uint16_t *len)
{
  char *start=NULL;
  char *end=NULL;
  start=strstr(src,s_str);
  if(start==NULL)
  {
   return 0;
  }
  end=strstr(start,e_str);
  if(end==NULL)
  {
    return 0;
  }
  *len=end-start+2;
  return (uint8_t*)start;
}

char *string_find_len(const char *str,uint16_t len,const char *sub)
{
   const char *pstr=str;
   char *start=(char*)str;
   const char *psub=sub;
   uint16_t slen=len;
   if((NULL == str)||(NULL == sub))
   {
       return NULL;
   }

   while(slen)
   {
       pstr=start;
       psub=sub;

       while(('\0' != *pstr)&&('\0' != *psub))
       {
           if(*pstr == *psub)
           {
               pstr ++;
               psub ++;
           }
           else
           {
               break;
           }
           slen --;
       }
       
       if('\0' == psub)
       {
           return start;
       }
       start ++;
   }
   return NULL;
}

void pb_AddNMEACheckSum(char *pBuf)
{
   unsigned char ckSum = 0;

   pBuf++; /* skip the $ sign */

   while (*pBuf != '\0')
   {
      ckSum ^= *pBuf++;
   }

   /* using sprintf since the size of pBuf gets modified before reaching here */
   sprintf(pBuf, "*%02X\r\n", ckSum);    /* +"*CK"*/
}
