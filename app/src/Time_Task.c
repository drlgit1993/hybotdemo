#include "time_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "syscfg.h"
#include "bsp_led.h"

char CPU_RunInfo[512];
static TimerHandle_t Stack_Test_Timer_Handle=NULL;


void Stack_Test_Timer_Callback(TimerHandle_t xTimer)
{
 memset(CPU_RunInfo,0,512);
vTaskList(CPU_RunInfo);
USART6_printf("\r\n");
USART6_printf("������                     ����״̬   ���ȼ�   ʣ��ջ  �������\r\n");
USART6_printf("%s",CPU_RunInfo);
USART6_printf("\r\n");


 memset(CPU_RunInfo,0,512);
vTaskGetRunTimeStats(CPU_RunInfo);
USART6_printf("������                       ���м���     ʹ����\r\n");
USART6_printf("%s",CPU_RunInfo);
USART6_printf("\r\n");
}
void Timer_Task_Create(void)
{ 
   Stack_Test_Timer_Handle = xTimerCreate("Stack_Test_Timer",
                pdMS_TO_TICKS(10000),
                pdTRUE,
                (void*)1,
                Stack_Test_Timer_Callback);

   if(NULL != Stack_Test_Timer_Handle)
   {
     xTimerStart(Stack_Test_Timer_Handle,0);
   }
}
