#include "usart_task.h"
#include "app_mem.h"
#include "disk_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "iap.h"

#define USART6_IN_MAX        USART6_RX_MAX

static SemaphoreHandle_t   USART6_BinarySem_Handle =NULL;
static TaskHandle_t        USART6_Task_Handle = NULL;

__align(4) static uint8_t     USART6_in_Buf[USART6_IN_MAX];


extern EventGroupHandle_t  disk_EventGroup_Handle;


static void USART6_Task(void *argv)
{
   uint16_t len=0;
   
   while (1)
   {
		     xSemaphoreTake(USART6_BinarySem_Handle,portMAX_DELAY); 
       memset(USART6_in_Buf,0,USART6_IN_MAX);
       len=USART6_Data_Receive(USART6_in_Buf);
       Iap_Handle(USART6_in_Buf,len);
       //Protocol_Parsing(USART6_Task_Buf,len);
       taskYIELD(); 
   }
}

void USART6_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   bsp_USART6_init();

   USART6_BinarySem_Handle=xSemaphoreCreateBinary();
   if(NULL == USART6_BinarySem_Handle)
   {
       CONSLOE_LOG(LOG_LVL_ERROR,"USART6 BinarySem create fail!\n");
       return ;
   }

   xReturn = xTaskCreate((TaskFunction_t )USART6_Task,  
                         (const char*    )USART6_TASK_NAME,
                         (uint16_t       )USART6_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )USART6_TASK_PRIO, 
                         (TaskHandle_t*  )&USART6_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"USART6 Task create fail!\n");
    return;
  }  
}

void USART6_Receive_Set(void)
{
   uint32_t ulReturn;
   BaseType_t pxHigherPriorityTaskWoken;
   ulReturn = taskENTER_CRITICAL_FROM_ISR();
   xSemaphoreGiveFromISR(USART6_BinarySem_Handle,&pxHigherPriorityTaskWoken);	//释放二值信号量
   portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}
