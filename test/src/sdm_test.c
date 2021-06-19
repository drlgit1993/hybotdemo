#include "sdm_test.h"

char buf[128]={"$SDM,1,1C002900095053424E353420,060180,000001,1000,0,0.0000000,0.0000000,0.0,0.0,0.0,0,0*"};
uint8_t temp[128]={0};

void sdm_test(void)
{
  static uint8_t cnt=0;

  EC20_output_t *NMEA=NMEA_send_handler_get();

  if(is_ec20_connected())
  {
     return ;
  }

  if(cnt<100)
  {
     if(NMEA->valid==1)
     {
       memset(temp,0,128);
       strcpy((char*)temp,buf);
       sprintf((char*)temp+strlen((char*)temp),"%d",cnt++);
       USART3_data_send((uint8_t *)temp,strlen((const char*)buf));
       EC20_output_start(NMEA,MSG_ID,temp,strlen((const char*)temp));
     }
   }
}
