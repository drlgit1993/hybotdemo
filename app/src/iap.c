#include "iap.h"
#include "FreeRTOS.h"
#include "task.h"

void Iap_Handle(void *pbuf,uint16_t len)
{
   char *ptemp=(char *)pbuf;
   uint16_t i=0;
   uint32_t boot=FLASH_FLAG_BOOT;
/*
#up$ 
*/
   for(i =0;i<len;i++)
   {
       if('#' != *ptemp++)
       {
           continue;
       }

       if(('u' == *ptemp++)&&('p' == *ptemp++)&&('$' == *ptemp))
       {
          taskENTER_CRITICAL();
          vTaskSuspendAll();
          backup_write_flash(UPGRADEaddr,&boot,1);

          xTaskResumeAll();
          taskEXIT_CRITICAL();

          system_reset();
       }
   }
}
