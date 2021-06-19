#include "xsz_protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hybotdemo.h"

#define PROTOCOL_HEADER                ((uint8_t)(0x55))
#define PROTOCOL_TAIL                  ((uint16_t)0xcccc)
#define FRAME_LEN_POS                  (6)
#define SERVER_FRAME_FUNC_CNT          (0x0b+1)  
#define OTA_FRAME_FUNC_CNT             (0x03+1)  
#define MCU_FRAME_FUNC_CNT             (0x07+1)  


/*error code*/
#define  POSHUB_FRAME_ERR_NONE         ((uint8_t)0x00)
#define  POSHUB_FRAME_ERR_GROUP        ((uint8_t)0x01)
#define  POSHUB_FRAME_ERR_FUNC         ((uint8_t)0x02)
#define  POSHUB_FRAME_ERR_ORDER        ((uint8_t)0x03)
#define  POSHUB_FRAME_ERR_LEN          ((uint8_t)0x04)
#define  POSHUB_FRAME_ERR_CFG          ((uint8_t)0x05)
#define  POSHUB_FRAME_ERR_TIMEOUT      ((uint8_t)0x06)
#define  POSHUB_FRAME_ERR_CRC          ((uint8_t)0x07)
#define  POSHUB_FRAME_ERR_EXE          ((uint8_t)0x08)

#define  POSHUB_FRAME_GROUP_LEN   ((uint8_t)(0x03-1))
#define  POSHUB_FRAME_DATA_OFFSET  (0x08)

typedef void(*poshub_func)(const void *,uint16_t);

typedef struct
{
   uint8_t *pbuf;
   uint16_t len;
}poshub_frame_cache_t;

typedef struct
{
   uint8_t header;
   uint8_t err;
   uint8_t group;
   uint8_t func;

   uint16_t order;
   uint16_t len;
}poshub_frame_t;

typedef struct
{
   uint8_t  buf[256];
   uint16_t len;
}poshub_frame_out_t;

static void Primary_Server_Config(const void *pdata,uint16_t len);
static void Primary_Server_Config_Get(const void *pdata,uint16_t len);
static void Message_Server_Config(const void *pdata,uint16_t len);
static void Message_Server_Config_Get(const void *pdata,uint16_t len);
static void OTA_Server_Config(const void *pdata,uint16_t len);
static void OTA_Server_Config_Get(const void *pdata,uint16_t len);
static void RTCM_Server_Config(const void *pdata,uint16_t len);
static void RTCM_Server_Config_Get(const void *pdata,uint16_t len);
static void Open_usb(const void *pdata,uint16_t len);
static void Close_usb(const void *pdata,uint16_t len);
static void Format_Disk(const void *pdata,uint16_t len);
static void Read_Disk(const void *pdata,uint16_t len);
static void Read_Mcu(const void *pdata,uint16_t len);

poshub_func   server_func[SERVER_FRAME_FUNC_CNT]=
{
   NULL,NULL,NULL,NULL,
   Primary_Server_Config,
   Primary_Server_Config_Get,
   Message_Server_Config,
   Message_Server_Config_Get,
   OTA_Server_Config,
   OTA_Server_Config_Get,
   RTCM_Server_Config,
   RTCM_Server_Config_Get
};
poshub_func   ota_func[OTA_FRAME_FUNC_CNT];
poshub_func   mcu_func[MCU_FRAME_FUNC_CNT]=
{
   Open_usb,
   Close_usb,
   Format_Disk,
   Read_Disk,
   NULL,
   NULL,
   NULL,
   Read_Mcu
};
uint8_t i2c_buf[512]={0};

poshub_func *Poshub_Frame_func[]={server_func,ota_func,mcu_func};

poshub_frame_cache_t frame_cache={NULL,0};
poshub_frame_out_t poshub_frame_out={0};
poshub_frame_t *pframe_output=NULL;
uint16_t last_order=0;

static uint32_t usb_flag     __attribute__ ((section(".no_init_data"),zero_init));
static uint8_t reset_en=0;
static void Poshub_Config_Frame_Respond(void)
{
   poshub_frame_out.buf[6]=0x00;
   poshub_frame_out.buf[7]=0x02;
   poshub_frame_out.len=10;
   *(uint16_t*)(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET)=(uint16_t)0x0000;     
}

uint8_t Is_usb_Open(void)
{
  return (usb_flag==0x55aa55aa);
}
static void Open_usb(const void *pdata,uint16_t len)
{
   usb_flag=0x55aa55aa;
   Poshub_Config_Frame_Respond();
   reset_en=1;
}
static void Close_usb(const void *pdata,uint16_t len)
{
   usb_flag=0x00000000;
   Poshub_Config_Frame_Respond();
   reset_en=1;
}
static void Format_Disk(const void *pdata,uint16_t len)
{
   Disk_Busy_Set();
   f_mkfs("0:",0,0,FF_MAX_SS);		/*¸ñÊ½»¯¿¨*/	
   Poshub_Config_Frame_Respond();
   reset_en=1;
}

static void Read_Disk(const void *pdata,uint16_t len)
{
	 uint8_t ret=0,buf_len=0;
	 char buf[64]={0};
	 disk_capatity_t capatity={0};
	 ret=disk_fatfs_capacity_get( &capatity);
	 if(ret == 1)
	 {
	     pframe_output->err=POSHUB_FRAME_ERR_EXE;
		   Poshub_Config_Frame_Respond();
		   return ;
	 }
	 sprintf(buf,"%d,%d;",capatity.total,capatity.available);
	 buf_len=strlen(buf);
   poshub_frame_out.len = 0x08+buf_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(buf_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)buf_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,buf,buf_len);
}

static void Read_Mcu(const void *pdata,uint16_t len)
{
	 uint8_t buf_len=0;
	 char buf[64]={0};

	 sprintf(buf,"%s,%s,%s;",POSHUBVERSION,COMPILE_TIME,COMPILE_DATE);
	 buf_len=strlen(buf);
   poshub_frame_out.len = 0x08+buf_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(buf_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)buf_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,buf,buf_len);
}

static void Primary_Server_Config(const void *pdata,uint16_t len)
{
   uint8_t pri_buf[PRI_SERVER_CONFIG_SIZE];
   uint8_t *pbuf=(uint8_t*)pdata;

   memset(pri_buf,0xff,PRI_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(PRI_SERVER_CONFIG_ADDR,pri_buf,PRI_SERVER_CONFIG_SIZE);

   bsp_i2c_Write(PRI_SERVER_CONFIG_ADDR,pbuf,len);
   Poshub_Config_Frame_Respond();
	 //reset_en=1;
}

static void Primary_Server_Config_Get(const void *pdata,uint16_t len)
{
   uint8_t pri_buf[PRI_SERVER_CONFIG_SIZE];
   uint8_t *buf=NULL;
   uint16_t valid_len=0;

   (void)pdata;
   (void)len;

   memset(pri_buf,0,PRI_SERVER_CONFIG_SIZE);
   bsp_i2c_read(PRI_SERVER_CONFIG_ADDR,pri_buf,PRI_SERVER_CONFIG_SIZE);
      
   buf=string_find("PRI",";",(const char*)pri_buf,&len);
   if(NULL == buf)
   {
       pframe_output->err=POSHUB_FRAME_ERR_CFG;
       Poshub_Config_Frame_Respond();
       return ;
   }

   while((pri_buf[valid_len]<0x80)&&(valid_len<PRI_SERVER_CONFIG_SIZE))
   {
       valid_len++;
   }

   valid_len=(valid_len<PRI_SERVER_CONFIG_SIZE)?valid_len:PRI_SERVER_CONFIG_SIZE;
   poshub_frame_out.len = 0x08+valid_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(valid_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)valid_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,pri_buf,valid_len);
}

static void Message_Server_Config(const void *pdata,uint16_t len)
{
	 uint8_t msg_buf[MSG_SERVER_CONFIG_SIZE];
   uint8_t *pbuf=(uint8_t*)pdata;

   memset(msg_buf,0xff,MSG_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(MSG_SERVER_CONFIG_ADDR,msg_buf,MSG_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(MSG_SERVER_CONFIG_ADDR,pbuf,len);
   Poshub_Config_Frame_Respond();
	 //reset_en=1;
}

static void Message_Server_Config_Get(const void *pdata,uint16_t len)
{
	 uint8_t msg_buf[MSG_SERVER_CONFIG_SIZE];
   uint8_t *buf=NULL;
   uint16_t valid_len=0;

   (void)pdata;
   (void)len;

	 memset(msg_buf,0,MSG_SERVER_CONFIG_SIZE);
   bsp_i2c_read(MSG_SERVER_CONFIG_ADDR,msg_buf,MSG_SERVER_CONFIG_SIZE);
	       
   buf=string_find("MSG",";",(const char*)msg_buf,&len);
   if(NULL == buf)
   {
       pframe_output->err=POSHUB_FRAME_ERR_CFG;
       Poshub_Config_Frame_Respond();
       return ;
   }

   while((msg_buf[valid_len]<0x80)&&(valid_len<MSG_SERVER_CONFIG_SIZE))
   {
       valid_len++;
   }

   valid_len=(valid_len<MSG_SERVER_CONFIG_SIZE)?valid_len:MSG_SERVER_CONFIG_SIZE;
   poshub_frame_out.len = 0x08+valid_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(valid_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)valid_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,msg_buf,valid_len);
}

static void OTA_Server_Config(const void *pdata,uint16_t len)
{
	 uint8_t ota_buf[MSG_SERVER_CONFIG_SIZE];
   uint8_t *pbuf=(uint8_t*)pdata;

   memset(ota_buf,0xff,OTA_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(OTA_SERVER_CONFIG_ADDR,ota_buf,OTA_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(OTA_SERVER_CONFIG_ADDR,pbuf,len);
   
   Poshub_Config_Frame_Respond();
	// reset_en=1;
}

static void OTA_Server_Config_Get(const void *pdata,uint16_t len)
{
   uint8_t ota_buf[OTA_SERVER_CONFIG_SIZE];
   uint8_t *buf=NULL;
   uint16_t valid_len=0;

   (void)pdata;
   (void)len;

	 memset(ota_buf,0,OTA_SERVER_CONFIG_SIZE);
   bsp_i2c_read(OTA_SERVER_CONFIG_ADDR,ota_buf,OTA_SERVER_CONFIG_SIZE);
	       
   buf=string_find("OTA",";",(const char*)ota_buf,&len);
   if(NULL == buf)
   {
       pframe_output->err=POSHUB_FRAME_ERR_CFG;
      Poshub_Config_Frame_Respond();
       return ;
   }

   while((ota_buf[valid_len]<0x80)&&(valid_len<OTA_SERVER_CONFIG_SIZE))
   {
       valid_len++;
   }

   valid_len=(valid_len<OTA_SERVER_CONFIG_SIZE)?valid_len:OTA_SERVER_CONFIG_SIZE;
   poshub_frame_out.len = 0x08+valid_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(valid_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)valid_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,ota_buf,valid_len);
}

static void RTCM_Server_Config(const void *pdata,uint16_t len)
{
	 uint8_t rtcm_buf[RTCM_SERVER_CONFIG_SIZE];
   uint8_t *pbuf=(uint8_t*)pdata;

   memset(rtcm_buf,0xff,RTCM_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(RTCM_SERVER_CONFIG_ADDR,rtcm_buf,RTCM_SERVER_CONFIG_SIZE);
   bsp_i2c_Write(RTCM_SERVER_CONFIG_ADDR,pbuf,len);
   
   Poshub_Config_Frame_Respond();
	 //reset_en=1;
}

static void RTCM_Server_Config_Get(const void *pdata,uint16_t len)
{
   uint8_t rtcm_buf[RTCM_SERVER_CONFIG_SIZE];
   uint8_t *buf=NULL;
   uint16_t valid_len=0;

   (void)pdata;
   (void)len;

	 memset(rtcm_buf,0,RTCM_SERVER_CONFIG_SIZE);
   bsp_i2c_read(RTCM_SERVER_CONFIG_ADDR,rtcm_buf,RTCM_SERVER_CONFIG_SIZE);
	       
   buf=string_find("RTCM",";",(const char*)rtcm_buf,&len);
   if(NULL == buf)
   {
       pframe_output->err=POSHUB_FRAME_ERR_CFG;
       Poshub_Config_Frame_Respond();
       return ;
   }

   while((rtcm_buf[valid_len]<0x80)&&(valid_len<RTCM_SERVER_CONFIG_SIZE))
   {
       valid_len++;
   }

   valid_len=(valid_len<RTCM_SERVER_CONFIG_SIZE)?valid_len:RTCM_SERVER_CONFIG_SIZE;
   poshub_frame_out.len = 0x08+valid_len;
   pframe_output->err=POSHUB_FRAME_ERR_NONE;
	 poshub_frame_out.buf[6]=(uint8_t)(valid_len>>8);
   poshub_frame_out.buf[7]=(uint8_t)valid_len;
   memcpy(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET,rtcm_buf,valid_len);
}

static uint8_t Poshub_Frame_Get(const void *psrc,uint16_t len)
{
   uint8_t *pbuf=(uint8_t*)psrc;
   uint16_t data_len=0,index=0,tail=0,crc16=0;

   memset(&frame_cache,0,sizeof(poshub_frame_cache_t));

   if((NULL == psrc) ||(len == 0))
   {
       return 1;
   }

   /*find frame header*/
   for(index=0;index<len;index++)
   {
       if(PROTOCOL_HEADER == *pbuf)
          break;
       else
           pbuf ++;     
   }

   if(index == len)
   {
       return 1;
   }

   /*find frame tail*/
   data_len=(uint16_t)(pbuf[FRAME_LEN_POS]<<8) + pbuf[FRAME_LEN_POS+1];
   tail=(pbuf[10+data_len]<<8 )+ pbuf[11+data_len];
   if(PROTOCOL_TAIL != tail)
   {
      return 1;
   }

   crc16=crc16_calc(pbuf,(data_len+10));
   if(0x00 != crc16)
   {
       return 1;
   }

   frame_cache.pbuf= (uint8_t *)pbuf;
   frame_cache.len=12+data_len;
   return 0;
}

static uint8_t Poshub_Frame_Group_check(void)
{
   poshub_frame_t *poshub_frame=NULL;

   poshub_frame=(poshub_frame_t*)frame_cache.pbuf;

   if(poshub_frame->group >POSHUB_FRAME_GROUP_LEN)
   {
      pframe_output->err=POSHUB_FRAME_ERR_GROUP;
      pframe_output->len=0x02;
      poshub_frame_out.len=10;
      *(uint16_t*)(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET)=(uint16_t)0x0000;     
      return 1;
   }
   return 0;
}

static uint8_t Poshub_Frame_Order_Check(void)
{
   poshub_frame_t *poshub_frame=NULL;

   poshub_frame=(poshub_frame_t*)frame_cache.pbuf;
   if((last_order != 0)&&(poshub_frame->order >last_order))
   {
       pframe_output->err=POSHUB_FRAME_ERR_ORDER;
       pframe_output->len=0x02;
       poshub_frame_out.len=10;
       *(uint16_t*)(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET)=(uint16_t)0x0000;
       return 1;
   }
   return 0;
}
static uint8_t Poshub_Frame_Length_Check(void)
{
   poshub_frame_t *poshub_frame=NULL;
   uint16_t len=0 ;
   poshub_frame=(poshub_frame_t*)frame_cache.pbuf;

   len=poshub_frame->len ;
   poshub_frame->len=((uint16_t)(0x00ff&len)<<8) | ((uint16_t)(0xff00&len)>>8);
   if(poshub_frame->len >1024)
   {
       pframe_output->err=POSHUB_FRAME_ERR_LEN;
       pframe_output->len=0x02;
       poshub_frame_out.len=10;
       *(uint16_t*)(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET)=(uint16_t)0x0000;
       return 1;
   }
   return 0;
}
static uint8_t Poshub_Frame_Func_check(void)
{
   poshub_frame_t *poshub_frame=NULL;

   poshub_frame=(poshub_frame_t*)frame_cache.pbuf;

   switch(poshub_frame->group)
   {
       case 0:
       {
           if(poshub_frame->func>0x0b)
           {
               goto FREAM_FUNC_ERR;
           }
       }break;

       case 1:
       {
           if(poshub_frame->func>0x03)
           {
               goto FREAM_FUNC_ERR;
           }
       }break;

       case 2:
       {
           if(poshub_frame->func>0x07)
           {
               goto FREAM_FUNC_ERR;
           }
       }break;
   }
   return 0;

FREAM_FUNC_ERR:
   pframe_output->err=POSHUB_FRAME_ERR_FUNC;
   pframe_output->len=0x02;
   poshub_frame_out.len=10;
   *(uint16_t*)(poshub_frame_out.buf+POSHUB_FRAME_DATA_OFFSET)=(uint16_t)0x0000;
   return 1;
}

static void Poshub_Frame_Handle(void)
{
   poshub_frame_t *poshub_frame=(poshub_frame_t *)frame_cache.pbuf;

   memset(&poshub_frame_out,0,sizeof(poshub_frame_out_t));

   pframe_output->header=PROTOCOL_HEADER;
   pframe_output->group=poshub_frame->group;
   pframe_output->func=poshub_frame->func;
   pframe_output->order=poshub_frame->order;
   last_order=pframe_output->order;
   if(Poshub_Frame_Group_check())
   {
     return ;
   }

   if(Poshub_Frame_Func_check())
   {
       return ;
   }

   if(Poshub_Frame_Order_Check())
   {
       return ;
   }

   if(Poshub_Frame_Length_Check())
   {
       return ;
   }

   pframe_output->err=POSHUB_FRAME_ERR_NONE;

   poshub_frame=(poshub_frame_t*)frame_cache.pbuf;
  
   Poshub_Frame_func[poshub_frame->group][poshub_frame->func](frame_cache.pbuf+8,poshub_frame->len);
}

static void Poshub_Frame_Respond(void)
{
   uint8_t *pbuf=poshub_frame_out.buf+poshub_frame_out.len;
   uint16_t crc16=0x00;

   crc16=crc16_calc(poshub_frame_out.buf,poshub_frame_out.len);
   *pbuf++=(uint8_t)crc16;
   *pbuf++=(uint8_t)(crc16>>8);
   *pbuf++=(uint8_t)PROTOCOL_TAIL;
   *pbuf=(uint8_t)(PROTOCOL_TAIL>>8);
   poshub_frame_out.len +=4;
   USART3_data_send(poshub_frame_out.buf,poshub_frame_out.len);
}

void Protocol_Parsing(const void *psrc,uint16_t len)
{
   if(Poshub_Frame_Get(psrc,len))
   {
       return ;
   }
   pframe_output=(poshub_frame_t*)(poshub_frame_out.buf);
   Poshub_Frame_Handle();
   Poshub_Frame_Respond();
   if(reset_en)
   {
		   vTaskDelay(pdMS_TO_TICKS(100));
       system_reset();
   }
}

