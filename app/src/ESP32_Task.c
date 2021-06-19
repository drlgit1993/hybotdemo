#include "esp32_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "syscfg.h"
#include "log.h"
#include "disk_task.h"

#define ESP32_IN_MAX         UART4_RX_MAX
#define ESP32_OUT_MAX        (1024)
#define WIFI_OUT_MAX         (1024)

typedef enum
{
  ESP32_HW_INIT_STATE_INITING       = 0,
  ESP32_HW_INIT_STATE_INITED        = 1
} ESP32_HW_Init_State_t;

static TimerHandle_t       ESP32_Scan_Timer_Handle=NULL;
static TimerHandle_t       ESP32_Send_Timer_Handle=NULL;
static EventGroupHandle_t  ESP32_EventGroup_Handle=NULL;
static SemaphoreHandle_t   ESP32_Out_MuxSem_Handle =NULL;

static TaskHandle_t        ESP32_Receive_Task_Handle = NULL;
static TaskHandle_t        Wifi_Scan_Task_Handle = NULL;
static TaskHandle_t        ESP32_Init_Task_Handle = NULL;

Que_t *ESP32_Output=NULL;
Que_t *ESP32_input=NULL;

__align(4) static uint8_t     ESP32_in_Buf[ESP32_IN_MAX];
__align(4) static uint8_t     ESP32_out_Buf[ESP32_OUT_MAX];
__align(4) static uint8_t     Wifi_info_Buf[WIFI_OUT_MAX];

typedef enum
{
   ESP32_AT_CHECK,         /*0、*/
   ESP32_SEND_AT,          /*1、发送AT指令*/
   BLE_ECHOES_CLOSE,       /*2、关闭回显*/
   BLE_INIT,               /*3、初始化BLE为SERVER*/
   BLE_GATTSSRVCRE,        /*4、GATTS创建服务*/
   BLE_GATTSSRVSTART,      /*5、GATTS开启服务*/
   BLE_NAME_SET,           /*6、设置BLE设备名称*/
   BLE_ADVDATA_SET,        /*7、设置广播名称*/
   BLE_ADVSRAER,           /*8、开启BLE广播*/
   WIFI_MODE_SET,          /*9、设置WIFI模式为Station*/
   WIFI_CWLAP_SET,         /*10、WIFI扫描结果参数设置*/
   ESP32_RESTART,          /*11、*/
   ESP32_INIT_IDLE,        /*12、*/
}ESP32_INIT_STEP;


typedef struct
{
    char *name;
    ESP32_INIT_STEP (*act)(void);                   
    ESP32_INIT_STEP success_step;
    char *cmd;                           /*命令*/
    char *ack;                           /*应答*/
    uint32_t  delay;                     /*uint:ms*/
    uint32_t  times;               
    uint32_t  repeat;
}ESP32_Init_t;

typedef struct
{
    uint8_t state;
    uint32_t  times;               
    uint32_t  repeat;
    ESP32_INIT_STEP cur_step;                       /*初始化步骤*/ 
    ESP32_INIT_STEP cmd_step;
    ESP32_Init_t init[ESP32_INIT_IDLE]; 
}ESP32_dev_t;

static ESP32_INIT_STEP ESP32_AT_Check(void);
static ESP32_INIT_STEP ESP32_Send_AT_Cmd(void);
static ESP32_INIT_STEP ESP32_Echoes_Close(void);
static ESP32_INIT_STEP ESP32_BLE_Init(void);
static ESP32_INIT_STEP ESP32_BLE_GATTS_Create(void);
static ESP32_INIT_STEP ESP32_BLE_GATTS_Start(void);
static ESP32_INIT_STEP ESP32_BLE_Name_Set(void);
static ESP32_INIT_STEP ESP32_BLE_Broadcast_Set(void);
static ESP32_INIT_STEP ESP32_BLE_Broadcast(void);
static ESP32_INIT_STEP ESP32_WIFI_Init(void);
static ESP32_INIT_STEP ESP32_WIFI_Scan_Set(void);

ESP32_dev_t  ESP32_dev=
{
   .state=ESP32_HW_INIT_STATE_INITING,
   .times=0,
   .repeat=0,
   .cur_step=ESP32_AT_CHECK,
   .cmd_step=ESP32_AT_CHECK,
   .init=
   {
							{"AT Check",ESP32_AT_Check,BLE_ECHOES_CLOSE,"AT\r\n","OK\r\n",500,40,5},
							/*1、发送AT指令*/
							{"Send AT Cmd",ESP32_Send_AT_Cmd,BLE_INIT," "," ",10,10,5},
							/*2、关闭回显*/      
							{"Echoes Close",ESP32_Echoes_Close,BLE_INIT,"ATE0\r\n","OK\r\n",500,40,5},
							/*3、初始化BLE为SERVER*/  
							{"BLE Init",ESP32_BLE_Init,BLE_GATTSSRVCRE,"AT+BLEINIT=2\r\n","OK\r\n",500,40,5},
       /*4、GATTS创建服务*/
							{"GATTS  Create",ESP32_BLE_GATTS_Create,BLE_GATTSSRVSTART,"AT+BLEGATTSSRVCRE\r\n","OK\r\n",200,10,5},
							/*5、GATTS开启服务*/
							{"GATTS  Start",ESP32_BLE_GATTS_Start,BLE_NAME_SET,"AT+BLEGATTSSRVSTART\r\n","OK\r\n",200,10,5},
							/*6、设置BLE设备名称*/
							{"BLE Name Set",ESP32_BLE_Name_Set,BLE_ADVDATA_SET,"AT+BLENAME=\"xszble\"\r\n","OK\r\n",200,10,5},
							/*7、设置广播名称*/
       {"BLE Advdata Set",ESP32_BLE_Broadcast_Set,BLE_ADVSRAER,"AT+BLEADVDATA=\"020106070978737a626c65\"\r\n","OK\r\n",200,10,5},
       /*8、开启BLE广播*/
       {"BLE Advstart",ESP32_BLE_Broadcast,WIFI_MODE_SET,"AT+BLEADVSTART\r\n","OK\r\n",200,10,5},
       /*9、设置WIFI模式为Station*/ 
       {"WIFI Mode Set",ESP32_WIFI_Init,WIFI_CWLAP_SET,"AT+CWMODE=1\r\n","OK\r\n",200,10,5},
       /*10、WIFI扫描结果参数设置*/ 
       {"WIFI Scan Set",ESP32_WIFI_Scan_Set,ESP32_INIT_IDLE,"AT+CWLAPOPT=0,12\r\n","OK\r\n",200,10,5},  
   }
};

static void ESP32_Output_mutex_lock(void)
{
    if (( NULL ==ESP32_Out_MuxSem_Handle) ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(ESP32_Out_MuxSem_Handle,portMAX_DELAY);
}

static void ESP32_Output_mutex_unlock(void)
{
    if (( NULL ==ESP32_Out_MuxSem_Handle) ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(ESP32_Out_MuxSem_Handle);
}

static void ESP32_Cmd_Set(ESP32_INIT_STEP cmd_step,uint8_t repeat)
{
   ESP32_dev.cmd_step=cmd_step;
   ESP32_dev.times=ESP32_dev.init[ESP32_dev.cmd_step].times;
   if(repeat)
   {
       ESP32_dev.repeat=ESP32_dev.init[ESP32_dev.cmd_step].repeat;
   }
}

static ESP32_INIT_STEP ESP32_Send_AT_Cmd(void)
{ 
   ESP32_INIT_STEP cmd_step=ESP32_dev.cmd_step;
   TickType_t  delay_tick=pdMS_TO_TICKS(ESP32_dev.init[ESP32_dev.cur_step].delay);
   ESP32_Send_Cmd(ESP32_dev.init[ESP32_dev.cmd_step].cmd);
   vTaskDelay(delay_tick);
   return cmd_step;
}

static ESP32_INIT_STEP ESP32_InitCmd_Ack_Wait(const char *str,TickType_t delay)
{
   if(ESP32_dev.times)
   {
       ESP32_dev.times --;
       CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> wait......\r\n",str);
       vTaskDelay(delay);
       return ESP32_dev.cur_step;
   }
   return ESP32_INIT_IDLE;
}

static ESP32_INIT_STEP ESP32_Cmd_Resend(const char *str,TickType_t delay)
{
   if(ESP32_dev.repeat)
   {
       ESP32_dev. repeat--;
       /*重新发指令*/
       CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> repeat\r\n",str);
       ESP32_Cmd_Set(ESP32_dev.cur_step,0);
       vTaskDelay(delay);
       return ESP32_SEND_AT;
   }
   return ESP32_INIT_IDLE;
}

static ESP32_INIT_STEP ESP32_Receive_Error_handle(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE;
   const char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   TickType_t  delay=pdMS_TO_TICKS(ESP32_dev.init[ESP32_dev.cur_step].delay);

   step=ESP32_InitCmd_Ack_Wait(str,delay);
   if(ESP32_INIT_IDLE !=step)
   {
       return step;
   }

   step=ESP32_Cmd_Resend(str,delay);
   if(ESP32_INIT_IDLE !=step)
   {
       return step;
   }

   CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> fail\r\n",str);
   ESP32_Cmd_Set(ESP32_RESTART,1);
   vTaskDelay(delay);
   return  ESP32_SEND_AT;
}

uint16_t ESP32_Receive_Date(uint32_t xTicksToWait)
{
   uint16_t len=0;
   if(xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_RECV,pdTRUE,pdTRUE,xTicksToWait))
   {
       memset(ESP32_in_Buf,0,ESP32_IN_MAX);
       len=Que_data_read(ESP32_input,ESP32_in_Buf);
       DeQueueEx(ESP32_input,len);
   }
   return len;
}

static ESP32_INIT_STEP ESP32_Cmd_Ack_Handle(char **dest,uint32_t xTicksToWait)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE;
   const char *str=ESP32_dev.init[ESP32_dev.cur_step].name;

   if(ESP32_Receive_Date(xTicksToWait))
   {   
       *dest=strstr((const char *)ESP32_in_Buf,ESP32_dev.init[ESP32_dev.cur_step].ack);
       if(NULL != *dest)
	      {
           return ESP32_SEND_AT;
	      }	
       else
       {
           step=ESP32_Cmd_Resend(str,xTicksToWait);
           if(ESP32_INIT_IDLE !=step)
           {
                return step;
           }
           CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> fail\r\n",str);
           ESP32_Cmd_Set(ESP32_RESTART,1);
           vTaskDelay(xTicksToWait);
           return  ESP32_SEND_AT;
       }
   }

   *dest=NULL;
   step=ESP32_Receive_Error_handle();

   return step;
}
static ESP32_INIT_STEP ESP32_AT_Check(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(50));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}


static ESP32_INIT_STEP ESP32_Echoes_Close(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(50));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;

}

static ESP32_INIT_STEP ESP32_BLE_Init(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_BLE_GATTS_Create(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_BLE_GATTS_Start(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_BLE_Name_Set(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_BLE_Broadcast_Set(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_BLE_Broadcast(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_WIFI_Init(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE,cmd_step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=ESP32_dev.init[ESP32_dev.cur_step].success_step;
     ESP32_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static ESP32_INIT_STEP ESP32_WIFI_Scan_Set(void)
{
   ESP32_INIT_STEP step=ESP32_INIT_IDLE;
   char *str=ESP32_dev.init[ESP32_dev.cur_step].name;
   char *dest=NULL;

   step=ESP32_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
       CONSLOE_LOG(LOG_LVL_INFO,"ESP32 Init Info:%s --> PASS",str);
       return ESP32_dev.init[ESP32_dev.cur_step].success_step; 
   }
   
   return step;
}

static uint8_t ESP32_Get_Wifi_Info(uint8_t *pbuf, uint16_t len, uint16_t *delen)
{
	  uint16_t wifi_info_len=0,search_len = len, header = 0,tail=0;
			memset(Wifi_info_Buf, 0x00, WIFI_OUT_MAX);
			wifi_info_len = sprintf((char*)Wifi_info_Buf, "$PWIFI,1,");
			uint8_t is_header = 0, is_tail = 0;
   
   if(0 == len)
			{
      return 0;
			}

			while (search_len)
			{
						 for (header = tail; header < len; header++)
						 {
										 if (pbuf[header] == '(')
										 {
											    is_header = 1;
											    break;
										 }
						 }

						 for (tail = header; tail < len; tail++)
						 {
											if (pbuf[tail] == ')')
											{
																is_tail = 1;
																break;
											}
						}
						if ((0 == is_header) || (0 == is_tail))
						{
										 break;
						}
						is_header = 0;
						is_tail = 0;
						search_len = len-tail;
						memcpy(Wifi_info_Buf + wifi_info_len, pbuf + header+1, tail - header-1 );
						wifi_info_len += tail - header - 1;
						Wifi_info_Buf[wifi_info_len] = ',';
						wifi_info_len += 1;
	  }
			Wifi_info_Buf[strlen((const char*)Wifi_info_Buf)-1]='\r';
   Wifi_info_Buf[strlen((const char*)Wifi_info_Buf)]='\n';
   Wifi_info_Buf[strlen((const char*)Wifi_info_Buf)+1]='\0';
			return 1;
}

static void ESP32_WIFI_Scan_Task(void)
{
   uint16_t len=0,delen=0;
   while(1)
   {
							xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_WIFI_SCAN,pdTRUE,pdTRUE,portMAX_DELAY);
							ESP32_Output_mutex_lock();
USART6_printf("wifi scan\r\n");
							ESP32_Send_Cmd("AT+CWLAP\r\n");

							xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_RECV,pdTRUE,pdTRUE,portMAX_DELAY);
							len=Que_data_read(ESP32_input,ESP32_in_Buf);
							delen=len;

							if(ESP32_Get_Wifi_Info(ESP32_in_Buf,len,&delen))
							{
											USART6_data_send(Wifi_info_Buf,strlen((char*)Wifi_info_Buf));
											DeQueueEx(ESP32_input,delen);
							}

							ESP32_Output_mutex_unlock();
   }
}

static uint8_t  ESP32_Cmd_Ack_Check(uint8_t *pbuf,uint16_t len,uint16_t *delen)
{
   uint16_t index=0;

   /*check  response string ">"*/

   for(index =0;index<len;index++)
   {
       if('>' != pbuf[index])
       {
           continue;
       }
       *delen=index+1;
       return 1;
   }
   return 0;
}

static void BLE_WIFI_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   
   BLE_WIFI_PWR_CLK_ENABLE();

   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

   GPIO_InitStruct.Pin = BLE_WIFI_PWR_PIN;
   HAL_GPIO_Init(BLE_WIFI_PWR_PORT, &GPIO_InitStruct);

   HAL_GPIO_WritePin(BLE_WIFI_PWR_PORT,BLE_WIFI_PWR_PIN,GPIO_PIN_SET);


   BLE_WIFI_EN_CLK_ENABLE();

   GPIO_InitStruct.Pin = BLE_WIFI_EN_PIN;
   HAL_GPIO_Init(BLE_WIFI_EN_PORT, &GPIO_InitStruct);

   HAL_GPIO_WritePin(BLE_WIFI_EN_PORT,BLE_WIFI_EN_PIN,GPIO_PIN_SET);  
}


static void ESP32_Receive_Task(void *argv)
{ 
   while(1)
   {
       xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_UART_RECV,pdTRUE,pdTRUE,portMAX_DELAY);
       UART4_Cnt_Get();
       UART4_Data_Printf();
       if(ESP32_HW_INIT_STATE_INITING == ESP32_dev.state)
							{
										 memset(ESP32_in_Buf,0x00,ESP32_IN_MAX);
										 UART4_Data_Copy(ESP32_in_Buf);
           if(NULL != strstr((char*)ESP32_in_Buf,"\r\nready\r\n"))
											{
               ESP32_dev.state=ESP32_HW_INIT_STATE_INITED;
											    UART4_Cnt_Flush();
               vTaskResume(ESP32_Init_Task_Handle);
											}						    
							}
							else
							{
											UART4_Data_EnQueue(ESP32_input);
							    UART4_Cnt_Flush();
 xEventGroupSetBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_RECV);
       }    
   }
}

static void ESP32_INIT_Task(void *argv)
{
   ESP32_dev.cur_step=ESP32_SEND_AT;
   ESP32_Cmd_Set(ESP32_AT_CHECK,1);

   while(1)
   {
      ESP32_dev.cur_step=ESP32_dev.init[ESP32_dev.cur_step].act();
      if(ESP32_INIT_IDLE == ESP32_dev.cur_step)
      {
             xTimerStart(ESP32_Scan_Timer_Handle,0);
             xTimerStart(ESP32_Send_Timer_Handle,0);
             vTaskResume(Wifi_Scan_Task_Handle);
             vTaskSuspend(NULL);
      }
   }
}

char ESP32_msg_name[32]={0};
char ESP32_sdm[128]={0};
static uint32_t ESP32_cnt=0;
uint8_t ESP32_name_send=0;
extern device_info_t device_hybot;

void ESP32_Output_Task(void *argv)
{
   uint16_t len=0,delen=0;
   char cmd[32]={0};

   while(1)
   {    
							xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_OUT,pdTRUE,pdTRUE,portMAX_DELAY);
       ESP32_Output_mutex_lock();

   if(ESP32_name_send==0)
   {
      ESP32_name_send=1;
      memset(ESP32_sdm,0x00,128);
      memcpy(ESP32_msg_name,device_hybot.NMEA_name,strlen(device_hybot.NMEA_name)-4);
      sprintf(ESP32_sdm,"$SDM,0,55004D001851363438383730,%s*5B\r\n",ESP32_msg_name);
      EnQueueEx(ESP32_Output,(uint8_t *)ESP32_sdm,strlen(ESP32_sdm));
   }
   memset(ESP32_sdm,0x00,128);
   sprintf(ESP32_sdm,"$SDM,1,4B00370010504D3550373320,270521,021149.900,2959000,1,34.1972180,108.8556149,0.0,0.0,360.0,1,%d*4B\r\n",ESP32_cnt);
   EnQueueEx(ESP32_Output,(uint8_t *)ESP32_sdm,strlen(ESP32_sdm));
   ESP32_cnt++;

							len=GetQueLength(ESP32_Output);
							len= len > 256 ? 256 : len ;
							if(0 == len)
							{ 
           ESP32_Output_mutex_unlock();
											break ;
							}

							sprintf(cmd,"AT+BLEGATTSNTFY=0,3,6,%d\r\n",len);

							ESP32_Send_Cmd(cmd);
							CpyQueData(ESP32_Output,ESP32_out_Buf,len);     
							USART6_printf("ESP32 send %d byte data\r\n",len);

							xEventGroupWaitBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_RECV,pdTRUE,pdTRUE,portMAX_DELAY);

							len=Que_data_read(ESP32_input,ESP32_in_Buf);
							delen=len;

							if(ESP32_Cmd_Ack_Check(ESP32_in_Buf,len,&delen))
							{
											UART4_data_DMA_send(ESP32_out_Buf,len);
											DeQueueEx(ESP32_Output,len);
											DeQueueEx(ESP32_input,delen);
							}
       ESP32_Output_mutex_unlock();
   }
}

void AIR724_Scan_Timer_Callback(TimerHandle_t xTimer)
{
   xEventGroupSetBits(ESP32_EventGroup_Handle,ESP32_EVENT_WIFI_SCAN);
}

void AIR724_Send_Timer_Callback(TimerHandle_t xTimer)
{
   //USART6_printf("esp32 send\r\n");
   //xEventGroupSetBits(ESP32_EventGroup_Handle,ESP32_EVENT_BLE_OUT);
}

void ESP32_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   BLE_WIFI_Pin_Init();
   bsp_UART4_init();

   ESP32_input=App_Mem_Get(ESP32_INPUT);
   ESP32_Output=App_Mem_Get(ESP32_OUTPUT);

			ESP32_Out_MuxSem_Handle=xSemaphoreCreateMutex();
			if(NULL ==ESP32_Out_MuxSem_Handle)
			{
							CONSLOE_LOG(LOG_LVL_ERROR,"ESP32 Out Mutex create fail!\n");
							return;
			}

   ESP32_Scan_Timer_Handle = xTimerCreate("AIR724 Timer",
                pdMS_TO_TICKS(5000),
                pdTRUE,
                (void*)1,
                AIR724_Scan_Timer_Callback);

   ESP32_Send_Timer_Handle = xTimerCreate("AIR724 Timer",
                pdMS_TO_TICKS(2000),
                pdTRUE,
                (void*)2,
                AIR724_Send_Timer_Callback);

   xReturn = xTaskCreate((TaskFunction_t )ESP32_Receive_Task,  
                         (const char*    )ESP32_RECEIVE_TASK_NAME,
                         (uint16_t       )ESP32_RECEIVE_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )ESP32_RECEIVE_TASK_PRIO, 
                         (TaskHandle_t*  )&ESP32_Receive_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"ESP32 Receive Task create fail!\n");
    goto ESP32_Task_Create_Fail;
  }  

   xReturn = xTaskCreate((TaskFunction_t )ESP32_WIFI_Scan_Task,  
                         (const char*    )WIFI_SCAN_TASK_NAME,
                         (uint16_t       )WIFI_SCAN_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )WIFI_SCAN_TASK_PRIO, 
                         (TaskHandle_t*  )&Wifi_Scan_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"ESP32 Wifi Scan Task create fail!\n");
    goto ESP32_Task_Create_Fail;
  } 

   vTaskSuspend(Wifi_Scan_Task_Handle);
   xReturn = xTaskCreate((TaskFunction_t )ESP32_INIT_Task,  
                         (const char*    )ESP32_INIT_TASK_NAME,
                         (uint16_t       )ESP32_INIT_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )ESP32_INIT_TASK_PRIO, 
                         (TaskHandle_t*  )&ESP32_Init_Task_Handle);
			if(pdFAIL == xReturn)
			{
					CONSLOE_LOG(LOG_LVL_ERROR,"ESP32 Task create fail!\n");
     goto ESP32_Task_Create_Fail;
			} 
   vTaskSuspend(ESP32_Init_Task_Handle);

			ESP32_EventGroup_Handle=xEventGroupCreate();	
			if(NULL == ESP32_EventGroup_Handle)
			{
       CONSLOE_LOG(LOG_LVL_ERROR,"ESP32 EventGroup Handle create fail");
						 goto ESP32_Task_Create_Fail;
			}

   return ;

			ESP32_Task_Create_Fail:
			{
       if(	ESP32_Receive_Task_Handle != NULL)
							{
											vTaskDelete(ESP32_Receive_Task_Handle);
											ESP32_Receive_Task_Handle=NULL;
							}

       if(	Wifi_Scan_Task_Handle != NULL)
							{
											vTaskDelete(Wifi_Scan_Task_Handle);
											Wifi_Scan_Task_Handle=NULL;
							}

       if(NULL != ESP32_Out_MuxSem_Handle)
			    { 
           vSemaphoreDelete(ESP32_Out_MuxSem_Handle);
           ESP32_Out_MuxSem_Handle=NULL;
       }

						return ;
			}
}

void ESP32_Receive_Set(void)
{
   uint32_t ulReturn;
   BaseType_t pxHigherPriorityTaskWoken;
   ulReturn = taskENTER_CRITICAL_FROM_ISR();
   xEventGroupSetBitsFromISR(ESP32_EventGroup_Handle,ESP32_EVENT_UART_RECV,&pxHigherPriorityTaskWoken);
   portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

