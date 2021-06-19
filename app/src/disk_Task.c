#include "disk_task.h"
#include "app_mem.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "syscfg.h"
#include "log.h"


#define DISK_TASK_BUFF_MAX         (USART1_RX_MAX)
device_info_t device_hybot;

static uint32_t usboremmc     __attribute__ ((section(".no_init_data"),zero_init));
__align(4) static uint8_t     Disk_Task_Buf[DISK_TASK_BUFF_MAX];

static uint8_t bsp_Disk_Init(void);

static TaskHandle_t        Disk_NMEA_Write_Task_Handle = NULL;
static EventGroupHandle_t  disk_EventGroup_Handle=NULL;
static SemaphoreHandle_t   disk_MuxSem_Handle=NULL;
Que_t *disk_nmea_input=NULL;

static void disk_mutex_lock(void)
{

    if ( (NULL == disk_MuxSem_Handle) || ( !FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(disk_MuxSem_Handle,portMAX_DELAY);
}

static void disk_mutex_unlock(void)
{
    if ( (NULL == disk_MuxSem_Handle) || ( !FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(disk_MuxSem_Handle);
}

void disk_nmea_EnQueue(void)
{ 
   if(IS_Disk_Ready(FIL_NMEA) && (NULL != disk_EventGroup_Handle))
   {         
      if(USART1_Data_EnQueue(disk_nmea_input))
      {    
           xEventGroupSetBits(disk_EventGroup_Handle,DISK_EVENT_NMEA_WRITE);   
      }      
   }
}

static void Disk_nmea_write_Task(void *argv)
{
   uint16_t len=0; 

   while (1)
   {
       xEventGroupWaitBits(disk_EventGroup_Handle,DISK_EVENT_NMEA_WRITE,pdTRUE,pdTRUE,portMAX_DELAY);

       if(GetQueLength(disk_nmea_input) > 512)
       {
           disk_mutex_lock();
           memset(Disk_Task_Buf,'*',DISK_TASK_BUFF_MAX);
											len=Que_data_read(disk_nmea_input,Disk_Task_Buf);
											DeQueueEx(disk_nmea_input,len);

											disk_fatfs_write(FIL_NMEA,Disk_Task_Buf,len);
           disk_mutex_unlock();
       }     
       taskYIELD();
   }
}

void Disk_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   if(bsp_Disk_Init())
			{
					  return ;
			}

   xReturn = xTaskCreate((TaskFunction_t )Disk_nmea_write_Task,  
                         (const char*    )DISK_NMEA_WRITE_TASK_NAME,
                         (uint16_t       )DISK_NMEA_WRITE_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )DISK_NMEA_WRITE_TASK_PRIO, 
                         (TaskHandle_t*  )&Disk_NMEA_Write_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"disk nmea writeTask create fail!\n");
  }

  disk_EventGroup_Handle=xEventGroupCreate();		
  disk_MuxSem_Handle=xSemaphoreCreateMutex();
  disk_nmea_input=App_Mem_Get(DISK_NMEA_INPUT);
}


static uint8_t bsp_device_init(void)
{
			uint32_t count=MX_RNG_Init();
			char name[32]={0};
   uint8_t rst=0;

			sprintf(device_hybot.NMEA_name,"%u.txt",count);
			sprintf(name,"%u.txt\r\n",count);
			device_hybot.valid=0;

   if(FR_OK == disk_fatfs_creat_file(device_hybot.NMEA_name,FA_CREATE_ALWAYS))
   {
       CONSLOE_LOG(LOG_LVL_INFO,"%s create success",device_hybot.NMEA_name);
   }
   else
   {
       rst ++;
   }

			if(FR_OK != disk_fatfs_creat_file("file_list.txt",FA_OPEN_APPEND|FA_WRITE))
			{
			    rst ++;
			}

			disk_fatfs_write(FIL_LIST,(uint8_t *)name,strlen(name));
			disk_close(FIL_LIST);
   return rst;
}

static uint8_t bsp_Disk_Init(void)
{
			if(bsp_disk_fatfs_init())
   {
       return 1;
   }
   if(2 == bsp_device_init())
   {
       return 2;
   }
   return 0;
}

