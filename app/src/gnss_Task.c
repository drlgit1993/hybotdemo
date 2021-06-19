#include "gnss_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "syscfg.h"
#include "ublox_cfg.h"
#include "disk_task.h"
#include "bsp_usart1.h"

static SemaphoreHandle_t   GNSS_BinarySem_Handle =NULL;
static TaskHandle_t        GNSS_Task_Handle = NULL;


static void GNSS_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   /*GNSS RST */
   GNSS_RST_CLK_ENABLE();
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Pin = GNSS_RST_PWR_PIN;
   HAL_GPIO_Init(GNSS_RST_PORT, &GPIO_InitStruct);
 HAL_GPIO_WritePin(GNSS_RST_PORT,GNSS_RST_PWR_PIN,GPIO_PIN_SET);

   /*GNSS EN */
   GNSS_EN_CLK_ENABLE();
   GPIO_InitStruct.Pin = GNSS_EN_PWR_PIN;
   HAL_GPIO_Init(GNSS_EN_PORT, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GNSS_EN_PORT,GNSS_EN_PWR_PIN,GPIO_PIN_SET);

   /*GNSS PPS */
/*
   GNSS_PPS_CLK_ENABLE();
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pin = GNSS_PPS_PWR_PIN;
   HAL_GPIO_Init(GNSS_PPS_PORT, &GPIO_InitStruct); 
   HAL_NVIC_SetPriority(GNSS_PPS_EXTI_IRQ, 5, 0);
   HAL_NVIC_EnableIRQ(GNSS_PPS_EXTI_IRQ);
*/
}

static void GNSS_Task(void *argv)
{

   while (1)
   {
		     xSemaphoreTake(GNSS_BinarySem_Handle,portMAX_DELAY); 

       USART1_Cnt_Get();
       //USART1_Data_Printf();
       disk_nmea_EnQueue();
       USART1_Cnt_Flush();
       vTaskDelay(10);         
   }
}

void GNSS_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   bsp_USART1_init();
   GNSS_Pin_Init();
   ublox_init_cfg();
   GNSS_BinarySem_Handle=xSemaphoreCreateBinary();
   if(NULL == GNSS_BinarySem_Handle)
   {
       CONSLOE_LOG(LOG_LVL_ERROR,"GNSS BinarySem create fail!\n");
       return ;
   }

   xReturn = xTaskCreate((TaskFunction_t )GNSS_Task,  
                         (const char*    )GNSS_TASK_NAME,
                         (uint16_t       )GNSS_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )GNSS_TASK_PRIO, 
                         (TaskHandle_t*  )&GNSS_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"GNSS Task create fail!\n");
  }
}

void GNSS_Receive_Date(void)
{
   uint32_t ulReturn;

   BaseType_t pxHigherPriorityTaskWoken;
			if(Is_USART1_Received() &&(NULL != GNSS_BinarySem_Handle))
			{
							ulReturn = taskENTER_CRITICAL_FROM_ISR();
							xSemaphoreGiveFromISR(GNSS_BinarySem_Handle,&pxHigherPriorityTaskWoken);	//释放二值信号量
							portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
							taskEXIT_CRITICAL_FROM_ISR( ulReturn );
    }
}

/*
pps与power off IO冲突

void GNSS_PPS_IRQHandler(void)
{
	  if(__HAL_GPIO_EXTI_GET_IT(GNSS_PPS_PWR_PIN) != RESET) 
	  {
		     __HAL_GPIO_EXTI_CLEAR_IT(GNSS_PPS_PWR_PIN);     
	  }  
}

*/
