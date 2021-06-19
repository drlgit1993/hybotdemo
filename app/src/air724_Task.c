#include "air724_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "syscfg.h"
#include "event_groups.h"
#include "disk_task.h"

#define AIR724_OUT_MAX           (1024*2)
#define AIR724_TASK_BUFF_MAX        USART2_RX_MAX
#define SERVER_IP_MAX               (32)

typedef enum
{
    AIR724_INIT_STEP_PWR_ON,                          /*0、模块上电*/
    AIR724_INIT_STEP_PWR_OFF,                         /*1、模块掉电*/
    AIR724_INIT_STEP_RESTART,                         /*2、模块重启*/
    AIR724_INIT_STEP_SEND_CMD,                        /*3、AIR724发送AT指令*/
    AIR724_INIT_STEP_SYNC,                            /*4、串口自适应同步*/
    AIR724_INIT_STEP_ECHOES_CLOSE,                    /*5、关闭回显*/
    AIR724_INIT_STEP_SET_RATE,                        /*6、设置波特率*/
    AIR724_INIT_STEP_SINGAL_CHECK,                    /*7、信号质量查询*/
    AIR724_INIT_STEP_REGISTERED_CHECK,                /*8、网络注册查询*/
    AIR724_INIT_STEP_ATTACH_CHECK,                    /*9、网络附着查询*/
    AIR724_INIT_STEP_CONNMXU_APN,                     /*10、设置多路连接*/
    AIR724_INIT_STEP_QSEND,                           /*11、设置快发模式*/
    AIR724_INIT_STEP_START_TASK,                      /*12、启动任务*/
    AIR724_INIT_STEP_CIICR,                           /*13、激活移动场景*/
    AIR724_INIT_STEP_OPEN_MSG,                        /*14、连接报文服务器*/
    AIR724_INIT_STEP_OPEN_OTA,                        /*15、连接OTA服务器*/ 	
    AIR724_INIT_STEP_CLOSE_MSG,                       /*16、关闭报文服务器*/
    AIR724_INIT_STEP_CLOSE_OTA,                       /*17、关闭OTA服务器*/
    AIR724_INIT_STEP_IDLE,                            /*18、*/
    AIR724_INIT_STEP_LEN
}AIR724_INIT_STEP;

typedef enum
{
   AIR724_OUTPUT_STEP_SEND_CMD,
   AIR724_OUTPUT_STEP_SEND_DATA,
   AIR724_OUTPUT_STEP_SEND_CHECK
}AIR724_OUTPUT_STEP;

typedef struct
{
   uint16_t id;
   uint16_t port;
   uint8_t ip[SERVER_IP_MAX];
}Server_Config_t;

typedef struct
{
   char open[64];
   char close[32];
   char open_ack[16];
   char close_ack[16];
}AIR724_Init_Cmd_t;

typedef struct
{ 
  Que_t * que;
  uint16_t send_len;
  uint8_t wait;
  uint8_t repeat;
  Server_Config_t config;
}AIR724_Socket_t;

typedef struct
{   
   AIR724_OUTPUT_STEP step;
   uint8_t id:3;
   uint16_t send_len;
   uint16_t accept_len;
   uint8_t wait;
   uint8_t repeat;
   Que_t * que;
   char out_cmd[32];
   AIR724_Socket_t socket[AIR724_SERVER_NUM];     
}AIR724_output_t;

AIR724_Init_Cmd_t AIR724_Init_Cmd[AIR724_SERVER_NUM]={0};

AIR724_output_t AIR724_output=
{
   .step=AIR724_OUTPUT_STEP_SEND_CMD,
   .id=0,
			.send_len=0,
			.accept_len=0,
   .wait=0,
   .repeat=0,
   .que=NULL,
   .out_cmd={0},
   .socket={
       {NULL,0,10,2,{AIR724_SERVER_MSG,1234,"120.27.92.24"}},
       {NULL,0,10,2,{AIR724_SERVER_OTA,1234,"120.27.92.24"}}
   }
};

typedef union
{
   struct
	  {
		     uint8_t SQ:1;          /*0、信号质量*/
		     uint8_t RG:1;          /*1、网络注册状态*/
		     uint8_t ATTACH:1;      /*2、网络附着状态*/
		     uint8_t MSG:1;         /*3、MSG服务器状态*/
		     uint8_t OTA:1;         /*4、OTA服务器状态*/
	  }state;
	  uint8_t status;
}AIR724_status_t;

typedef struct
{   
    char *name;
    AIR724_INIT_STEP (*act)(void);                   
    AIR724_INIT_STEP success_step;
    char *cmd;                           /*命令*/
    char *ack;                           /*应答*/
    uint32_t  delay;                     /*uint:ms*/
    uint32_t  times;               
    uint32_t  repeat;
}AIR724_connect_t;

typedef enum
{   
   AIR724_WAIT_ACK ,
   AIR724_WAIT_SEND,     
   AIR724_OUTPUT_IDLE,
} AIR724_State_t;

typedef struct
{   
    AIR724_State_t state;
    uint32_t  times;               
    uint32_t  repeat;
    uint8_t   restart;                      /*AIR724重启的次数*/
    uint8_t   deact_times;                  /*反激活的次数*/  
    AIR724_INIT_STEP cur_step;                       /*初始化步骤*/ 
    AIR724_INIT_STEP cmd_step;
    uint16_t connect;
    AIR724_status_t init_status;        /*ec20初始化状态*/
    AIR724_connect_t init[AIR724_INIT_STEP_LEN]; 
}AIR724_dev_t;



 TimerHandle_t       AIR724_Timer_Handle=NULL;

 EventGroupHandle_t  AIR724_EventGroup_Handle=NULL;

 TaskHandle_t        AIR724_Init_Task_Handle = NULL;
 TaskHandle_t        AIR724_Receive_Task_Handle = NULL;
 TaskHandle_t        MSG_OUTPUT_Task_Handle=NULL;
 SemaphoreHandle_t   AIR724_Out_MuxSem_Handle =NULL;

__align(4) static uint8_t     AIR724_Task_Buf[AIR724_TASK_BUFF_MAX];
__align(4) static uint8_t     AIR724_out_Buf[AIR724_OUT_MAX];
static Que_t *AIR724_input=NULL;

static AIR724_INIT_STEP AIR724_Pwr_On(void);
static AIR724_INIT_STEP AIR724_Pwr_Off(void);
static AIR724_INIT_STEP AIR724_Rst(void);
static AIR724_INIT_STEP AIR724_Send_AT_Cmd(void);
static AIR724_INIT_STEP AIR724_AT_Check(void);
static AIR724_INIT_STEP AIR724_Echoes_Close(void);
static AIR724_INIT_STEP AIR724_Baudrate_Set(void);
static AIR724_INIT_STEP AIR724_Signal_Check(void);
static AIR724_INIT_STEP AIR724_Registered_Check(void);
static AIR724_INIT_STEP AIR724_Attach_Check(void);
static AIR724_INIT_STEP AIR724_ConnectMux_set(void);
static AIR724_INIT_STEP AIR724_Qsend_Set(void);
static AIR724_INIT_STEP AIR724_Start_Task(void);
static AIR724_INIT_STEP AIR724_CIICR(void);
static AIR724_INIT_STEP AIR724_Message_Server_Open(void);
static AIR724_INIT_STEP AIR724_OTA_Server_Open(void);
static AIR724_INIT_STEP AIR724_Message_Server_Close(void);
static AIR724_INIT_STEP AIR724_OTA_Server_Close(void);

static AIR724_dev_t AIR724_dev=
{
   .state=AIR724_OUTPUT_IDLE,
   .times=0,
   .repeat=0,
   .restart=0,
   .deact_times=0,
   .cur_step=AIR724_INIT_STEP_PWR_ON, 
   .cmd_step=AIR724_INIT_STEP_IDLE,
   .connect=0,
   .init_status=0,
   .init=
   {
       /*0、模块上电*/
       {"pwr on",AIR724_Pwr_On,AIR724_INIT_STEP_SEND_CMD,NULL,NULL,1500,1,0},
       /*1、模块掉电*/   
       {"pwr off",AIR724_Pwr_Off,AIR724_INIT_STEP_PWR_ON,NULL,NULL,2000,1,0},  
       /*2、模块复位*/ 
       {"air742 rst",AIR724_Rst,AIR724_INIT_STEP_PWR_ON,NULL,NULL,1000,5,0}, 
       /*3、发送AT指令*/   
       {"send AT",AIR724_Send_AT_Cmd,AIR724_INIT_STEP_IDLE,NULL,NULL,100,1,0},  
       /*4、串口自适应同步*/
       {"com  sync",AIR724_AT_Check,AIR724_INIT_STEP_ECHOES_CLOSE,"AT\r\n","\r\nOK\r\n",100,5,10},  
       /*5、关闭回显*/
       {"echoes close",AIR724_Echoes_Close,AIR724_INIT_STEP_SINGAL_CHECK,"ATE0\r\n","\r\nOK\r\n",100,5,10},   
       /*6、设置波特率*/                
       {"Baudrate Set",AIR724_Baudrate_Set,AIR724_INIT_STEP_SINGAL_CHECK,"AT+IPR=115200\r\n","\r\nOK\r\n",100,5,10},   
       /*7、信号查询*/  
       {"Signal Check",AIR724_Signal_Check,AIR724_INIT_STEP_REGISTERED_CHECK,"AT+CSQ\r\n","+CSQ: ",100,5,20},                    
      /*8、网络注册查询*/
       {"register Check",AIR724_Registered_Check,AIR724_INIT_STEP_ATTACH_CHECK,"AT+CREG?\r\n","+CREG: ",2000,5,10},                         
       /*9、网络附着查询*/
       {"attach Check",AIR724_Attach_Check,AIR724_INIT_STEP_CONNMXU_APN,"AT+CGATT?\r\n","+CGATT: ",100,5,5},                     
       /*10、设置多路连接*/
       {"connect mux set",AIR724_ConnectMux_set,AIR724_INIT_STEP_QSEND,"AT+CIPMUX=1\r\n","OK\r\n",100,5,5}, 
        /*11、设置快发模式*/
       {"quick send",AIR724_Qsend_Set,AIR724_INIT_STEP_START_TASK,"AT+CIPQSEND=1\r\n","OK\r\n",100,200,5},     
       /*12、启动任务*/
       {"start task ",AIR724_Start_Task,AIR724_INIT_STEP_CIICR,"AT+CSTT\r\n","OK\r\n",100,10,10},                
       /*13、激活移动场景*/
       {"CIICR task ",AIR724_CIICR,AIR724_INIT_STEP_OPEN_MSG,"AT+CIICR\r\n","OK\r\n",100,10,10},                    
       /*14、连接报文服务器*/                   
       {"Message Server Open",AIR724_Message_Server_Open,AIR724_INIT_STEP_IDLE,AIR724_Init_Cmd[AIR724_SERVER_MSG].open,AIR724_Init_Cmd[AIR724_SERVER_MSG].open_ack,100,5,10}, 
       /*15、连接OTA服务器*/                     
       {"OTA Server Open",AIR724_OTA_Server_Open,AIR724_INIT_STEP_IDLE,AIR724_Init_Cmd[AIR724_SERVER_OTA].open,AIR724_Init_Cmd[AIR724_SERVER_OTA].open_ack,100,5,10},              
       /*16、关闭报文服务器*/                   
       {"Message Server Close",AIR724_Message_Server_Close,AIR724_INIT_STEP_OPEN_MSG,AIR724_Init_Cmd[AIR724_SERVER_MSG].close,AIR724_Init_Cmd[AIR724_SERVER_MSG].close_ack,100,5,10},        
       /*17、关闭更新固件服务器*/                  
       {"OTA Server Close",AIR724_OTA_Server_Close,AIR724_INIT_STEP_OPEN_OTA,AIR724_Init_Cmd[AIR724_SERVER_OTA].close,AIR724_Init_Cmd[AIR724_SERVER_OTA].close_ack,100,5,10},                           			   
   }
};


void AIR724_output_linkptr(AIR724_SERVER server,Que_t *que)
{
   if (NULL==que)
   {
     return ;
   }
   AIR724_output.socket[server].que=que;
}

static void AIR724_Cmd_Set(AIR724_INIT_STEP cmd_step,uint8_t repeat)
{
   AIR724_dev.cmd_step=cmd_step;
   AIR724_dev.times=AIR724_dev.init[AIR724_dev.cmd_step].times;
   if(repeat)
   {
       AIR724_dev.repeat=AIR724_dev.init[AIR724_dev.cmd_step].repeat;
   }
}
static AIR724_INIT_STEP AIR724_InitCmd_Ack_Wait(const char *str,TickType_t delay)
{
   if(AIR724_dev.times)
   {
       AIR724_dev.times --;
       CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> wait......\r\n",str);
       vTaskDelay(delay);
       return AIR724_dev.cur_step;
   }
   return AIR724_INIT_STEP_IDLE;
}

static AIR724_INIT_STEP AIR724_Cmd_Resend(const char *str,TickType_t delay)
{
   uint32_t repeat=AIR724_dev.init[AIR724_dev.cur_step].repeat;

   if(AIR724_dev.repeat)
   {     
       /*重新发指令*/
       CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> repeat %d\r\n",str,(repeat-AIR724_dev.repeat+1));
       AIR724_dev. repeat--;
       AIR724_Cmd_Set(AIR724_dev.cur_step,0);
       vTaskDelay(delay);
       return AIR724_INIT_STEP_SEND_CMD;
   }
   return AIR724_INIT_STEP_IDLE;
}

static AIR724_INIT_STEP AIR724_Receive_Error_handle(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   const char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   TickType_t  delay=pdMS_TO_TICKS(AIR724_dev.init[AIR724_dev.cur_step].delay);

   step=AIR724_InitCmd_Ack_Wait(str,delay);
   if(AIR724_INIT_STEP_IDLE !=step)
   {
       return step;
   }

   step=AIR724_Cmd_Resend(str,delay);
   if(AIR724_INIT_STEP_IDLE !=step)
   {
       return step;
   }

   CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> fail\r\n",str);
   AIR724_Cmd_Set(AIR724_INIT_STEP_RESTART,1);
   vTaskDelay(delay);
   return  AIR724_INIT_STEP_SEND_CMD;
}

uint16_t AIR724_Receive_Date(uint32_t xTicksToWait)
{
   uint16_t len=0;

   if(xEventGroupWaitBits(AIR724_EventGroup_Handle,AIR724_EVENT_RECV,pdTRUE,pdTRUE,xTicksToWait))
   {
       memset(AIR724_Task_Buf,0,AIR724_TASK_BUFF_MAX);
       len=Que_data_read(AIR724_input,AIR724_Task_Buf);
       DeQueueEx(AIR724_input,len);
   }
   return len;
}

static AIR724_INIT_STEP AIR724_Cmd_Ack_Handle(char **dest,uint32_t xTicksToWait)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   const char *str=AIR724_dev.init[AIR724_dev.cur_step].name;

   if(AIR724_Receive_Date(xTicksToWait))
   {   
       *dest=strstr((const char *)AIR724_Task_Buf,AIR724_dev.init[AIR724_dev.cur_step].ack);
       if(NULL != *dest)
	      {
           return AIR724_INIT_STEP_SEND_CMD;
	      }	
       else
       {
           step=AIR724_Cmd_Resend(str,xTicksToWait);
           if(AIR724_INIT_STEP_IDLE !=step)
           {
                return step;
           }
           CONSLOE_LOG(LOG_LVL_INFO,"EC20 Init Info:%s --> fail\r\n",str);
           AIR724_Cmd_Set(AIR724_INIT_STEP_RESTART,1);
           vTaskDelay(xTicksToWait);
           return  AIR724_INIT_STEP_SEND_CMD;
       }
   }

   *dest=NULL;
   step=AIR724_Receive_Error_handle();

   return step;
}

static AIR724_INIT_STEP AIR724_Send_AT_Cmd(void)
{ 
   AIR724_INIT_STEP cmd_step=AIR724_dev.cmd_step;
   TickType_t  delay_tick=pdMS_TO_TICKS(AIR724_dev.init[AIR724_dev.cur_step].delay);
   USART6_printf("%s\r\n",AIR724_dev.init[AIR724_dev.cmd_step].cmd);
   EC20_Send_cmd(AIR724_dev.init[AIR724_dev.cmd_step].cmd);
   vTaskDelay(delay_tick);
   return cmd_step;
}
static AIR724_INIT_STEP AIR724_Pwr_On(void)
{
   AIR724_PWRKEY_ON();
   CONSLOE_LOG(LOG_LVL_INFO,"PWRKEY ON");
   vTaskDelay(1000);
   AIR724_PWRKEY_OFF();
   CONSLOE_LOG(LOG_LVL_INFO,"PWRKEY OFF");
   AIR724_Cmd_Set(AIR724_INIT_STEP_SYNC,1);
   vTaskDelay(2000);
   
   return AIR724_dev.init[AIR724_dev.cur_step].success_step ;
}

static AIR724_INIT_STEP AIR724_Pwr_Off(void)
{
   return AIR724_dev.init[AIR724_dev.cur_step].success_step ;
}

static AIR724_INIT_STEP AIR724_Rst(void)
{
   return AIR724_dev.init[AIR724_dev.cur_step].success_step ;
}

static AIR724_INIT_STEP AIR724_AT_Check(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static AIR724_INIT_STEP AIR724_Echoes_Close(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static AIR724_INIT_STEP AIR724_Baudrate_Set(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static AIR724_INIT_STEP AIR724_Signal_Handle(void *pbuf)
{
   char *ptemp=(char*)pbuf;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   TickType_t  delay_tick=pdMS_TO_TICKS(AIR724_dev.init[AIR724_dev.cur_step].delay);
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   int rssi=0;
   
   sscanf(ptemp,"%*[^ ]%*c%d",&rssi);

   if((0<= rssi)&&(rssi<=31))
	  {	
			    AIR724_dev.init_status.state.SQ=AIR724_INIT_OK;
	    	 CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS(%d dBm)",str,(rssi<<1) - 113);  
       step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
       AIR724_Cmd_Set(step,1);
       return AIR724_INIT_STEP_SEND_CMD;     
		 }

   AIR724_dev.init_status.state.SQ=AIR724_INIT_ERR;
	  CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> Not known or not detectable",str);

   step=AIR724_Cmd_Resend(str,delay_tick);
   if(AIR724_INIT_STEP_IDLE !=step)
   {
       return step;
   }
   CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> fail\r\n",str);
   AIR724_Cmd_Set(AIR724_INIT_STEP_RESTART,1);
   vTaskDelay(delay_tick);
   return  AIR724_INIT_STEP_SEND_CMD;  
}

static AIR724_INIT_STEP AIR724_Signal_Check(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));
   (void)str;

   if(NULL != dest)
   {
     step=AIR724_Signal_Handle(dest);   
   }
   
   return step;
}

static AIR724_INIT_STEP AIR724_Registered_Handle(void *pbuf)
{
   char *ptemp=(char*)pbuf;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   int state=0;
/*+CREG: <n>,<stat>\r\n*/
   sscanf(ptemp,"%*[^,],%d",&state);

   (void)str;
   if((1==state)||(5==state)||(6==state)||(7==state))
	  {	
			    AIR724_dev.init_status.state.SQ=AIR724_INIT_OK;
	    	 CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);  
       step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
       AIR724_Cmd_Set(step,1);
       return AIR724_INIT_STEP_SEND_CMD;     
		 }

   AIR724_dev.init_status.state.SQ=AIR724_INIT_ERR;
	  CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> Not known or not detectable",str);

   step=AIR724_Receive_Error_handle();
   return step;
}

static AIR724_INIT_STEP AIR724_Registered_Check(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   (void)str;
   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     step=AIR724_Registered_Handle(dest);   
   }
   
   return step;
}

static AIR724_INIT_STEP AIR724_Attach_Handle(void *pbuf)
{
   char *ptemp=(char*)pbuf;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   int state=0;
  /*+CGATT: <state>\r\n*/
   sscanf(ptemp,"%*[^ ] %d",&state);
  (void)str;

   if(1==state)
	  {	
			    AIR724_dev.init_status.state.SQ=AIR724_INIT_OK;
	    	 CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);  
       step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
       AIR724_Cmd_Set(step,1);
       return AIR724_INIT_STEP_SEND_CMD;     
		 }

   AIR724_dev.init_status.state.SQ=AIR724_INIT_ERR;

   step=AIR724_Receive_Error_handle();
   return step;
}

static AIR724_INIT_STEP AIR724_Attach_Check(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     step=AIR724_Attach_Handle(dest);   
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_ConnectMux_set(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_Qsend_Set(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_Start_Task(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_CIICR(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
     EC20_Send_cmd("AT+CIFSR\r\n");
     vTaskDelay(200);
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_Message_Server_Open(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char already_cmd[32]={0};

   if(AIR724_Receive_Date(100))
   {   
       
       if(NULL != strstr((const char *)AIR724_Task_Buf,AIR724_dev.init[AIR724_dev.cur_step].ack))
	      {
											cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
											AIR724_Cmd_Set(cmd_step,1);
											CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
           return AIR724_INIT_STEP_SEND_CMD;
	      }	

       if(NULL != strstr((const char *)AIR724_Task_Buf,"\r\nOK\r\n"))
	      {
											step=AIR724_Receive_Error_handle();
											return step;
       }

       memset(already_cmd,0x00,32);
       sprintf(already_cmd,"%d, ALREADY CONNECT\r\n",AIR724_SERVER_MSG);
       if(NULL != strstr((const char *)AIR724_Task_Buf,already_cmd))
	      {
											cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
											AIR724_Cmd_Set(cmd_step,1);
											CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
           return AIR724_INIT_STEP_SEND_CMD;
	      }	
   }

   step=AIR724_Receive_Error_handle();
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_OTA_Server_Open(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char already_cmd[32]={0};

   if(AIR724_Receive_Date(100))
   {   
       
       if(NULL != strstr((const char *)AIR724_Task_Buf,AIR724_dev.init[AIR724_dev.cur_step].ack))
	      {
											cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
											AIR724_Cmd_Set(cmd_step,1);
											CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
           return AIR724_INIT_STEP_SEND_CMD;
	      }	

       if(NULL != strstr((const char *)AIR724_Task_Buf,"\r\nOK\r\n"))
	      {
											step=AIR724_Receive_Error_handle();
											return step;
       }

       memset(already_cmd,0x00,32);
       sprintf(already_cmd,"%d, ALREADY CONNECT\r\n",AIR724_SERVER_MSG);
       if(NULL != strstr((const char *)AIR724_Task_Buf,already_cmd))
	      {
											cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
											AIR724_Cmd_Set(cmd_step,1);
											CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
           return AIR724_INIT_STEP_SEND_CMD;
	      }	
   }

   step=AIR724_Receive_Error_handle();
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_Message_Server_Close(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   (void)str;
   return step;
}

static AIR724_INIT_STEP AIR724_OTA_Server_Close(void)
{
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE,cmd_step=AIR724_INIT_STEP_IDLE;
   char *str=AIR724_dev.init[AIR724_dev.cur_step].name;
   char *dest=NULL;

   step=AIR724_Cmd_Ack_Handle(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=AIR724_dev.init[AIR724_dev.cur_step].success_step;
     AIR724_Cmd_Set(cmd_step,1);
     CONSLOE_LOG(LOG_LVL_INFO,"AIR724 Init Info:%s --> PASS",str);
   }
   (void)str;
   return step;
}

uint8_t IS_AIR724_MSG_Server_Open(void)
{
   return (AIR724_dev.init_status.state.MSG==AIR724_INIT_OK);
}

static void AIR724_Init_Cmd_Set(Server_Config_t *config)
{
   /*AT+CIPSTART=<n>,<mode>,<IPaddress>,<port>*/
   sprintf(AIR724_Init_Cmd[config->id].open,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r\n",config->id,config->ip,config->port);
   sprintf(AIR724_Init_Cmd[config->id].open_ack,"%d, CONNECT OK\r\n",config->id);
   sprintf(AIR724_Init_Cmd[config->id].close,"AT+CIPCLOSE=%d,1\r\n",config->id);
   sprintf(AIR724_Init_Cmd[config->id].close_ack,"%d,CLOSE OK\r\n",config->id);
}

static void AIR724_Init_Config_Get(void)
{
   Server_Config_t *config=NULL;
   uint16_t offset=0,loop=0;
   AIR724_INIT_STEP step=AIR724_INIT_STEP_IDLE;

   AIR724_dev.connect =0x00;
   /*message server ip and port*/
   config=& AIR724_output.socket[AIR724_SERVER_MSG].config;

   AIR724_Init_Cmd_Set(config);
   AIR724_dev.connect |= SERVER_MSG_Msk;
/*
   config=& AIR724_output.socket[AIR724_SERVER_OTA].config;
  
   AIR724_Init_Cmd_Set(config);

   AIR724_dev.connect |= SERVER_OTA_Msk;  
*/
   for(offset=SERVER_MSG_Pos;offset<=SERVER_OTA_Pos;offset++)
   {
       if(AIR724_dev.connect & (1<<offset))
       {
         loop ++;
       }
   }

   for(offset=SERVER_MSG_Pos;offset<=SERVER_OTA_Pos;offset++)
   {
       if(AIR724_dev.connect & (1<<offset))
       {
           AIR724_dev.init[AIR724_INIT_STEP_CIICR].success_step=(AIR724_INIT_STEP)(0x0E + offset);   
           break;  
       }
   }
 
   if(offset>SERVER_OTA_Pos)
   {
       AIR724_dev.init[AIR724_INIT_STEP_CIICR].success_step=AIR724_INIT_STEP_IDLE;
       return ;
   }

   loop --;

   while(loop)
   {
       step=(AIR724_INIT_STEP)(0x0E+offset);
       for(offset++;offset<=SERVER_OTA_Pos;offset++)
       {
           if(AIR724_dev.connect & (1<<offset))
           {
               AIR724_dev.init[step].success_step=(AIR724_INIT_STEP)(0x0E+offset);   
               loop --;
               break;  
           }
       }
   }

   AIR724_dev.init[(0x0E+offset)].success_step= AIR724_INIT_STEP_IDLE;
}
static void AIR724_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   AIR724_PWRKEY_CLK_ENABLE();

   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Pin = AIR724_PWRKEY_PIN;
   HAL_GPIO_Init(AIR724_PWRKEY_PORT, &GPIO_InitStruct);
   AIR724_PWRKEY_OFF();

   AIR724_RST_CLK_ENABLE();
   GPIO_InitStruct.Pin = AIR724_RST_PIN;
   HAL_GPIO_Init(AIR724_RST_PORT, &GPIO_InitStruct);
   AIR724_RST_OFF();

   AIR724_PWR_CLK_ENABLE();
   GPIO_InitStruct.Pin = AIR724_PWR_PIN;
   HAL_GPIO_Init(AIR724_PWR_PORT, &GPIO_InitStruct);
   AIR724_PWR_ON();
}

static void AIR724_Init_Task(void *argv)
{
   while(1)
   {
       AIR724_dev.cur_step=AIR724_dev.init[AIR724_dev.cur_step].act();/*connect server*/

       if(AIR724_INIT_STEP_IDLE == AIR724_dev.cur_step)
       {
  xTimerStart(AIR724_Timer_Handle,0);
           vTaskSuspend(NULL);
       }
   }
}

void  AIR724_Cmd_Ack_Check(uint8_t *pbuf,uint16_t len,uint16_t *delen)
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
      xEventGroupSetBits(AIR724_EventGroup_Handle,AIR724_EVENT_ACK);
   }
}
static void AIR724_Receive_Task(void *argv)
{
   uint16_t len=0,delen=0;
   while (1)
   {

       xEventGroupWaitBits(AIR724_EventGroup_Handle,AIR724_EVENT_UART_RECV,pdTRUE,pdTRUE,portMAX_DELAY);
							USART2_Cnt_Get();
							USART2_Data_Printf();

							USART2_Data_EnQueue(AIR724_input);
							USART2_Cnt_Flush();

							if(AIR724_WAIT_ACK ==AIR724_dev.state)
							{
											memset(AIR724_Task_Buf,0,AIR724_TASK_BUFF_MAX);
											len=Que_data_read(AIR724_input,AIR724_Task_Buf);
											delen=len;
											AIR724_Cmd_Ack_Check(AIR724_Task_Buf,len,&delen);
											DeQueueEx(AIR724_input,delen);
							}
							else
       {
           xEventGroupSetBits(AIR724_EventGroup_Handle,AIR724_EVENT_RECV);    
       }
   }
}

void AIR724_Receive_Set(void)
{
   uint32_t ulReturn;
   BaseType_t pxHigherPriorityTaskWoken;
   ulReturn = taskENTER_CRITICAL_FROM_ISR();
   xEventGroupSetBitsFromISR(AIR724_EventGroup_Handle,AIR724_EVENT_UART_RECV,&pxHigherPriorityTaskWoken);
   portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}
static void AIR724_Output_mutex_lock(void)
{

    if (( NULL ==AIR724_Out_MuxSem_Handle) ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(AIR724_Out_MuxSem_Handle,portMAX_DELAY);
}

static void AIR724_Output_mutex_unlock(void)
{
    if (( NULL ==AIR724_Out_MuxSem_Handle) ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(AIR724_Out_MuxSem_Handle);
}

AIR724_OUTPUT_STEP AIR724_Send_Data(void)
{
   if(	xEventGroupWaitBits(AIR724_EventGroup_Handle,AIR724_EVENT_ACK,pdTRUE,pdTRUE,10))
   {
       AIR724_dev.state= AIR724_WAIT_SEND;
							USART2_data_send(AIR724_out_Buf,AIR724_output.send_len);     
       return AIR724_OUTPUT_STEP_SEND_CHECK;
   }

   if(AIR724_output.wait)
   {
      AIR724_output.wait --;
      return AIR724_OUTPUT_STEP_SEND_DATA;
   }

   if(AIR724_output.repeat)
   {
       AIR724_output.wait=AIR724_output.socket[AIR724_output.id].wait;
       AIR724_output.repeat --;
       USART6_printf("WARNING :No Write Ack, Send Cmd \"%s\" Again\r\n",AIR724_output.out_cmd);
       EC20_Send_cmd(AIR724_output.out_cmd);
       return  AIR724_OUTPUT_STEP_SEND_DATA;
   }

			USART2_data_send(AIR724_out_Buf,AIR724_output.send_len);
		
   AIR724_dev.state=AIR724_OUTPUT_IDLE;
   DeQueueEx(AIR724_output.que,AIR724_output.send_len);
			USART6_printf("ERROR:Write Cmd  \"%s\"Ack Timeout\r\n",AIR724_output.out_cmd);
			AIR724_Output_mutex_unlock();
			return AIR724_OUTPUT_STEP_SEND_CMD;
}

uint8_t AIR724_Data_Accept_Check(uint8_t *pbuf,uint16_t len,uint16_t *delen)
{
   uint16_t index=0, ack_len=0;
   const char *urc_read="DATA ACCEPT:";
   int server_id=0;
   char *header=NULL;
   char *tail=NULL;
   int  accept_len=0;

			header=strstr((const char*)pbuf,urc_read);
			if(NULL != header)
			{
							tail=strstr(header,"\r\n");
							if(NULL == tail)
							{
											*delen = header-(char*)pbuf;
											return 0;
							}

							sscanf(header,"%*[^:]:%d%*c%d",&server_id,&accept_len);   
							AIR724_output.accept_len = accept_len;
       *delen = tail-(char*)pbuf+1;
							return 1;
			}
   
   /*未搜索到$MYURCREAD: id\r\n,则检测末尾是否存在部分可能的响应指令*/
	  for (index=(len > 12 ? (len - 12) : 0); index<len; index++)
	  {
		     if (*urc_read == pbuf[index])
		     {
			        urc_read++;
			        ack_len++;
		     }
	  }
   
   if(ack_len)
   {
       *delen=len - ack_len;
   }
   return 0;
}

uint8_t AIR724_Send_Fail_Check(uint8_t *pbuf,uint16_t len,uint16_t *delen)
{
   uint16_t index=0, ack_len=0;
   const char *urc_read="SEND FAIL\r\n";
   char *header=NULL;

			header=strstr((const char*)pbuf,urc_read);
			if(NULL != header)
			{
       *delen = header-(char*)pbuf+11;
							return 1;
			}
   
   /*未搜索到$MYURCREAD: id\r\n,则检测末尾是否存在部分可能的响应指令*/
	  for (index=(len > 11 ? (len - 11) : 0); index<len; index++)
	  {
		     if (*urc_read == pbuf[index])
		     {
			        urc_read++;
			        ack_len++;
		     }
	  }
   
   if(ack_len)
   {
       *delen=len - ack_len;
   }
   return 0;
}

AIR724_OUTPUT_STEP AIR724_Send_Check(void)
{
   uint16_t len=0,delen=0;
   xEventGroupWaitBits(AIR724_EventGroup_Handle,AIR724_EVENT_RECV,pdTRUE,pdTRUE,portMAX_DELAY);
			memset(AIR724_Task_Buf,0,AIR724_TASK_BUFF_MAX);
			len=Que_data_read(AIR724_input,AIR724_Task_Buf);
			delen=len;

			if(AIR724_Data_Accept_Check(AIR724_Task_Buf,len,&delen))
			{
       if(AIR724_output.accept_len != AIR724_output.send_len)
							{
           USART6_printf("%d receive fail,send again\r\n",AIR724_output.id);
											USART2_data_send(AIR724_out_Buf,AIR724_output.send_len);     
											return AIR724_OUTPUT_STEP_SEND_CHECK;
							}
       AIR724_dev.state=AIR724_OUTPUT_IDLE;
       DeQueueEx(AIR724_output.que,AIR724_output.send_len);
       DeQueueEx(AIR724_input,delen);
       USART6_printf("%d receive ok\r\n",AIR724_output.id);
       AIR724_Output_mutex_unlock();
       return AIR724_OUTPUT_STEP_SEND_CMD;
			}
			
   if(AIR724_Send_Fail_Check(AIR724_Task_Buf,len,&delen))
			{
       USART6_printf("%d send fail,send again\r\n",AIR724_output.id);
       USART2_data_send(AIR724_out_Buf,AIR724_output.send_len);     
       return AIR724_OUTPUT_STEP_SEND_CHECK;
			}
   return AIR724_OUTPUT_STEP_SEND_CHECK;
}

void AIR724_MSG_output_Task(void *argv)
{
  uint32_t r_event = 0;  
   static  AIR724_OUTPUT_STEP step=AIR724_OUTPUT_STEP_SEND_CMD;
   uint16_t send_len=0;

   while(1)
   {    
       switch(step)
       {
           case AIR724_OUTPUT_STEP_SEND_CMD:
           {
               r_event=xEventGroupWaitBits(AIR724_EventGroup_Handle,AIR724_EVENT_SEND_NORMAL|AIR724_EVENT_SEND_BACKUP,pdTRUE,pdFALSE,portMAX_DELAY);

               AIR724_Output_mutex_lock();
                         
															if(r_event&AIR724_EVENT_SEND_BACKUP)
															{
																	// sdm_cache_read();
															}   

															AIR724_output.que=AIR724_output.socket[AIR724_SERVER_MSG].que;
															send_len=GetQueLength(AIR724_output.que);
															AIR724_output.send_len= send_len > 1460 ? 1460 : send_len ;
															if(0 == AIR724_output.send_len)
															{ 
																				AIR724_Output_mutex_unlock();
																				break ;
															}
		
															CpyQueData(AIR724_output.que,AIR724_out_Buf,AIR724_output.send_len);     

               AIR724_output.id=AIR724_SERVER_MSG;
															AIR724_output.wait=AIR724_output.socket[AIR724_SERVER_MSG].wait;
															AIR724_output.repeat=AIR724_output.socket[AIR724_SERVER_MSG].repeat;
               AIR724_dev.state=AIR724_WAIT_ACK;
															step=AIR724_OUTPUT_STEP_SEND_DATA;
															sprintf(AIR724_output.out_cmd,"AT+CIPSEND=%d,%d\r\n",AIR724_SERVER_MSG,send_len);
															EC20_Send_cmd(AIR724_output.out_cmd);
               USART6_printf("%d server send %d byte data\r\n",AIR724_SERVER_MSG,AIR724_output.send_len);
           }break;

											case AIR724_OUTPUT_STEP_SEND_DATA:
											{
																step=AIR724_Send_Data();
											}break;

											case AIR724_OUTPUT_STEP_SEND_CHECK:
											{
																step=AIR724_Send_Check();
											}break;
       }
   }
}
/*
+RECEIVE,<n>,<length>:
Received data*/
void AIR724_Socket_Receive_Handle(void *pbuf,uint16_t len)
{
   int socket_id=0;
   int data_len=0;
   char *ptemp=NULL;
   ptemp=strstr(pbuf,"+RECEIVE");
   if(NULL == ptemp)
   {
       return ;
   }
   
   data_len=len-(ptemp-(char*)pbuf);
   sscanf(ptemp, "%*[^,],%d%*c%d", &socket_id,&data_len);

   if(IS_AIR724_SERVER((AIR724_SERVER)socket_id))
   {
       //EC20_URC_receive_Callback[server_id](ptemp,data_len);
   }
}


char msg_name[32]={0};
char sdm[128]={0};
static uint32_t cnt=0;
uint8_t name_send=0;
extern device_info_t device_hybot;

void AIR724_Timer_Callback(TimerHandle_t xTimer)
{
   Que_t *MSG_out=App_Mem_Get(MSG_OUTPUT);

   if(name_send==0)
   {
      name_send=1;
      memset(sdm,0x00,128);
      memcpy(msg_name,device_hybot.NMEA_name,strlen(device_hybot.NMEA_name)-4);
      sprintf(sdm,"$SDM,0,55004D001851363438383730,%s*5B\r\n",msg_name);
      EnQueueEx(MSG_out,(uint8_t *)sdm,strlen(sdm));
   }
   memset(sdm,0x00,128);
   sprintf(sdm,"$SDM,1,4B00370010504D3550373320,270521,021149.900,2959000,1,34.1972180,108.8556149,0.0,0.0,360.0,1,%d*4B\r\n",cnt);
   EnQueueEx(MSG_out,(uint8_t *)sdm,strlen(sdm));
   cnt++;
   
   xEventGroupSetBits(AIR724_EventGroup_Handle,AIR724_EVENT_SEND_NORMAL);
}

void AIR724_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   AIR724_Init_Config_Get();
   if(0==AIR724_dev.connect)
   {
       return ;
   }

   AIR724_Pin_Init();

   bsp_USART2_init();

   AIR724_Timer_Handle = xTimerCreate("AIR724 Timer",
                pdMS_TO_TICKS(2000),
                pdTRUE,
                (void*)1,
                AIR724_Timer_Callback);

   if(NULL != AIR724_Timer_Handle)
   {
     
   }

   AIR724_input=App_Mem_Get(AIR724_INPUT);

   AIR724_EventGroup_Handle=xEventGroupCreate();	
			if(NULL == AIR724_EventGroup_Handle)
			{
       CONSLOE_LOG(LOG_LVL_ERROR,"AIR724 EventGroup Handle create fail");
						 goto AIR724_Task_Create_Fail;
			}

  xReturn = xTaskCreate((TaskFunction_t )AIR724_Receive_Task,  
                         (const char*    )AITR724_RECEIVE_TASK_NAME,
                         (uint16_t       )AITR724_RECEIVE_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )AITR724_RECEIVE_TASK_PRIO, 
                         (TaskHandle_t*  )&AIR724_Receive_Task_Handle);
  if(pdFAIL == xReturn)
  {
    CONSLOE_LOG(LOG_LVL_ERROR,"USART2 Task create fail!\n");
    goto AIR724_Task_Create_Fail;
  } 

   if(AIR724_dev.connect)
   {
       AIR724_Out_MuxSem_Handle=xSemaphoreCreateMutex();
       if(NULL ==AIR724_Out_MuxSem_Handle)
       {
           CONSLOE_LOG(LOG_LVL_ERROR,"AIR724 Out Mutex create fail!\n");
           return;
       }

							xReturn = xTaskCreate((TaskFunction_t )AIR724_Init_Task,  
																													(const char*    )AITR724_INIT_TASK_NAME,
																													(uint16_t       )AITR724_INIT_STACK_SIZE,  
																													(void*          )NULL,
																													(UBaseType_t    )AITR724_INIT_TASK_PRIO, 
																													(TaskHandle_t*  )&AIR724_Init_Task_Handle);
						if(pdFAIL == xReturn)
						{
								CONSLOE_LOG(LOG_LVL_ERROR,"AIR724UG Init Task create fail!\n");
								goto AIR724_Task_Create_Fail;
						}  

						//vTaskSuspend(AIR724_Init_Task_Handle);
   }

			if(AIR724_dev.connect & SERVER_MSG_Msk)
			{
							xReturn = xTaskCreate((TaskFunction_t )AIR724_MSG_output_Task,  
																													(const char*    )AIR724_MSG_TASK_NAME,
																													(uint16_t       )AIR724_MSG_TASK_STACK_SIZE,  
																													(void*          )NULL,
																													(UBaseType_t    )AIR724_MSG_TASK_PRIO, 
																													(TaskHandle_t*  )&MSG_OUTPUT_Task_Handle);
									if(pdFAIL == xReturn)
									{
													CONSLOE_LOG(LOG_LVL_ERROR,"EC20 Main Task create fail!\n");
													goto AIR724_Task_Create_Fail;
									}
			}
   return ;

			AIR724_Task_Create_Fail:
			{
       if(NULL != AIR724_Receive_Task_Handle)
       {
										 vTaskDelete(AIR724_Receive_Task_Handle);
										 AIR724_Receive_Task_Handle=NULL;
       }

       if(NULL != AIR724_Init_Task_Handle)
       {
										 vTaskDelete(AIR724_Init_Task_Handle);
										 AIR724_Init_Task_Handle=NULL;
       }

			}

}
