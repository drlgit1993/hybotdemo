#include "bsp_sdio_test.h"
#include "diskio.h"	
#include "diskio.h"
#include "bsp_usart6.h"

char *wbuff={"$GNRMC,065259.000,A,3411.81588,N,12345,10851.35720,E,,0.0,W,A*34\r\n$GNGSA,A,3,12,12,20,12345,25,23,24,21,122,,33,,4444,,1.79,1.13,1.39*19\r\n$GNGSA,A,3,407,406,416,,,,,,,,,,1.79,1.13,1.39*29\r\n$GPGSV,3,1,12,12,31,094,42,20,58,145,42,25,31,23,62,131,39*7C\r\n$GPGSV,3,2,12,24,28,046,36,21,14,312,32,10,82,338,26,32,47,303,26*76\r\n$GPGSV,3,3,12,31,22,222,23,18,12,182,21,11,02,310,,15,05,086,*76\r\n$BDGSV,2,1,08,407,24,157,37,401,35,128,34,404,2312,115,29*67\r\n$BDGSV,2,2,08,416,66,002,25,413,41,205,16,402,43,22,77,1,*67\r\n"};

uint8_t rbuf[512]={0};
uint16_t end=100;

void bsp_MMC_sdio_test(void)
{
  //HAL_Delay(1000);

  static uint16_t waddr=0,index=50;

	if(waddr>=end)
	{
	  return;
	}

 if (waddr==0)
 {
  printf("write start!\r\n");
 }

bsp_MMC_Init();

for(index=0;index<50;index++)
{
   memset(rbuf,0,512);
   disk_read(0,rbuf,index,1);
}

for(index=0;index<50;index++)
{
disk_write(0,(uint8_t*)wbuff,index,1);

   memset(rbuf,0,512);
   disk_read(0,rbuf,index,1);
}
}
