#include "ec20_task.h"
#include "rtcm_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "poshub.h"

#define EC20_IN_MAX            USART2_RX_MAX
#define EC20_OUT_MAX           (1024)
#define EC20_WAIT_TIMES        (20)/*20��*/
#define EC20_WAIT_DELAY        (20)/*20ms*/
#define EC20_EMPTY             (0)


static TaskHandle_t EC20_INIT_Task_Handle = NULL;
 TaskHandle_t SDM_OUTPUT_Task_Handle = NULL;
 TaskHandle_t RTCM_OUTPUT_Task_Handle = NULL;
SemaphoreHandle_t   EC20_BinarySem_Handle =NULL;
extern SemaphoreHandle_t   RTCM_BinarySem_Handle;
static SemaphoreHandle_t   EC20_MuxSem_Handle =NULL;

static uint8_t EC20_input_buf[EC20_IN_MAX]={0};
static uint8_t EC20_out_buf[EC20_OUT_MAX]={0};

typedef enum
{
   EC20_OUTPUT_STEP_START,
   EC20_OUTPUT_STEP_WAIT_ACK,
}EC20_OUTPUT_STEP;

EC20_OUTPUT_STEP EC20_output_start(void);
EC20_OUTPUT_STEP EC20_output_wait_ack(void);
EC20_OUTPUT_STEP EC20_output_query(void);
typedef struct
{ 
  Que_t * que;
  uint16_t send_len;
  Server_Config_t config;
}EC20_device_t;

typedef struct
{   
  EC20_OUTPUT_STEP step;
  uint8_t ack;
  uint8_t wait;
  uint8_t repeat;
  EC20_device_t *pdev;
  EC20_device_t device[EC20_SERVER_NUM];     
}EC20_output_t;

EC20_output_t EC20_output=
{
   .step=EC20_OUTPUT_STEP_START,
   .ack=0,
   .wait=10,
   .repeat=2,
   .pdev=NULL,
   .device=
   {
       {NULL,0,{{0},"PRI Server","PRI_ON",EC20_SERVER_PRI,PRI_SERVER_CONFIG_ADDR,PRI_SERVER_CONFIG_SIZE,0}},
       {NULL,0,{{0},"MSG Server","MSG_ON",EC20_SERVER_MSG,MSG_SERVER_CONFIG_ADDR,MSG_SERVER_CONFIG_SIZE,0}},
       {NULL,0,{{0},"OTA Server","OTA_ON",EC20_SERVER_OTA,OTA_SERVER_CONFIG_ADDR,OTA_SERVER_CONFIG_SIZE,0}},
       {NULL,0,{{0},"RTCM Server","RTCM_ON",EC20_SERVER_RTCM,RTCM_SERVER_CONFIG_ADDR,RTCM_SERVER_CONFIG_SIZE,0}}
   },
};

typedef enum
{
    EC20_INIT_STEP_BAT_ON,                          /*0��ģ���ϵ�*/
    EC20_INIT_STEP_BAT_OFF,                         /*1��ģ�����*/
    EC20_INIT_STEP_PWRKEY_LOW,                      /*2��PWRKEY����*/
    EC20_INIT_STEP_PWRKEY_HIGH,                     /*3��PWRKEY����*/
    EC20_INIT_STEP_SEND_CMD,                        /*4��EC20����ATָ��*/
    EC20_INIT_STEP_SYNC,                            /*5����������Ӧͬ��*/
    EC20_INIT_STEP_ECHOES_CLOSE,                    /*6���رջ���*/
    EC20_INIT_STEP_SET_RATE,                        /*7�����ò�����*/
    EC20_INIT_STEP_SIM_CHECK,                       /*8��SIM�����*/
    EC20_INIT_STEP_SINGAL_CHECK,                    /*9���źŲ�ѯ*/
    EC20_INIT_STEP_GSM_CHECK,                       /*10��GSM�źŲ�ѯ*/
    EC20_INIT_STEP_GPRS_CHECK,                      /*11��GPRS�źŲ�ѯ*/
    EC20_INIT_STEP_SET_APN,                         /*12������GPRS��APN*/
    EC20_INIT_STEP_DEACT_PDP,                       /*13��ȥ����PDP*/
    EC20_INIT_STEP_ACT_PDP,                         /*14������PDP*/
    EC20_INIT_STEP_OPEN_PRI,                        /*15������һ��������*/
    EC20_INIT_STEP_OPEN_MSG,                        /*16�����ӱ��ķ�����*/
    EC20_INIT_STEP_OPEN_OTA,                        /*17������OTA������*/
	  	EC20_INIT_STEP_OPEN_RTCM,                       /*18������RTCM������*/
    EC20_INIT_STEP_CLOSE_PRI,                       /*19���ر�һ��������*/
    EC20_INIT_STEP_CLOSE_MSG,                       /*20���رձ��ķ�����*/
    EC20_INIT_STEP_CLOSE_OTA,                       /*21���ر�OTA������*/
    EC20_INIT_STEP_CLOSE_RTCM,                      /*22���ر�RTCM������*/
    EC20_INIT_STEP_RESTART,                         /*23��ģ������*/
    EC20_INIT_STEP_IDLE,                            /*24��*/
    EC20_INIT_STEP_LEN
}EC20_INIT_STEP;

static EC20_INIT_STEP  EC20_Bat_On(void);
static EC20_INIT_STEP  EC20_Bat_Off(void);
static EC20_INIT_STEP  EC20_pwrkey_low(void);
static EC20_INIT_STEP  EC20_pwrkey_high(void);
static EC20_INIT_STEP  EC20_send_AT(void);
static EC20_INIT_STEP  EC20_AT_Sync_Check(void);
static EC20_INIT_STEP  EC20_Echoes_Close_Check(void);
static EC20_INIT_STEP  EC20_Baudrate_Set_Check(void);
static EC20_INIT_STEP  EC20_Sim_Check(void);
static EC20_INIT_STEP  EC20_Signal_Check(void);
static EC20_INIT_STEP  EC20_Network_Status_Check(void);
static EC20_INIT_STEP  EC20_GPRS_Check(void);
static EC20_INIT_STEP  EC20_GPRSAPN_Set(void);
static EC20_INIT_STEP  EC20_PDP_Deactivate(void);
static EC20_INIT_STEP  EC20_PDP_Activate(void);
static EC20_INIT_STEP  EC20_Primary_Server_Open(void);
static EC20_INIT_STEP  EC20_Primary_Server_Close(void);
static EC20_INIT_STEP  EC20_Message_Server_Open(void);
static EC20_INIT_STEP  EC20_Message_Server_Close(void);
static EC20_INIT_STEP  EC20_OTA_Server_Open(void);
static EC20_INIT_STEP  EC20_OTA_Server_Close(void);
static EC20_INIT_STEP  EC20_RTCM_Server_Open(void);
static EC20_INIT_STEP  EC20_RTCM_Server_Close(void);
static EC20_INIT_STEP  EC20_Restart(void);
static EC20_INIT_STEP EC20_Generic_init(char **dest,uint32_t xTicksToWait);

typedef struct
{   
    char *name;
    EC20_INIT_STEP (*act)(void);                   
    EC20_INIT_STEP success_step;
    char *cmd;                           /*����*/
    char *ack;                           /*Ӧ��*/
    uint32_t  delay;                     /*uint:ms*/
    uint32_t  times;               
    uint32_t  repeat;
}EC20_connect_t;

typedef struct
{
   char open[64];
   char close[16];
   char ack[16];
}EC20_Init_Cmd_t;

EC20_Init_Cmd_t EC20_Init_Cmd[EC20_SERVER_NUM]={0};

typedef struct
{
    uint32_t  times;               
    uint32_t  repeat;
    uint8_t   restart;                      /*EC20�����Ĵ���*/
    EC20_INIT_STEP cur_step;                       /*��ʼ������*/ 
    EC20_INIT_STEP cmd_step;
    uint16_t connect;
    ec20_status_t init_status;        /*ec20��ʼ��״̬*/
    EC20_connect_t init[EC20_INIT_STEP_LEN]; 
}EC20_dev_t;

/*EC20��ʱʱ��10ms*/
 static EC20_dev_t EC20_dev=
{
   .times=0,
   .repeat=0,
   .restart=0,
   .cur_step=EC20_INIT_STEP_BAT_ON, 
   .cmd_step=EC20_INIT_STEP_IDLE,
   .connect=0,
   .init_status=0,
   .init=
   {
       {"bat on",EC20_Bat_On,EC20_INIT_STEP_PWRKEY_LOW,NULL,NULL,300,1,0},/*0��ģ���ϵ�*/
       {"EC20_Bat_Off",EC20_Bat_Off,EC20_INIT_STEP_BAT_ON,NULL,NULL,2000,1,0},/*1��ģ�����*/
       {"pwrkey low",EC20_pwrkey_low,EC20_INIT_STEP_PWRKEY_HIGH,NULL,NULL,1000,1,0},/*2��PWRKEY����*/
       {"pwrkey high",EC20_pwrkey_high,EC20_INIT_STEP_SEND_CMD,NULL,NULL,100,1,0},   /*3��PWRKEY����*/
       {"send AT",EC20_send_AT,EC20_INIT_STEP_IDLE,NULL,NULL,100,1,0},                        /*4��EC20����ATָ��*/
       {"com  sync",EC20_AT_Sync_Check,EC20_INIT_STEP_ECHOES_CLOSE,"AT\r\n","OK\r\n",20,10,10},  /*5����������Ӧͬ��*/
       {"echoes close",EC20_Echoes_Close_Check,EC20_INIT_STEP_SET_RATE,"ATE0\r\n","OK\r\n",20,10,10},                    /*6���رջ���*/
       {"Baudrate Set",EC20_Baudrate_Set_Check,EC20_INIT_STEP_SIM_CHECK,"AT+IPR=115200\r\n","OK\r\n",20,10,10},                        /*7�����ò�����*/
       {"Sim Check",EC20_Sim_Check,EC20_INIT_STEP_SINGAL_CHECK,"AT+CPIN?\r\n","+CPIN: READY",50,10,10},                       /*8��SIM�����*/
       {"Signal Check",EC20_Signal_Check,EC20_INIT_STEP_GSM_CHECK,"AT+CSQ\r\n","+CSQ: ",20,10,20},                    /*9���źŲ�ѯ*/
       {"Network Status Check",EC20_Network_Status_Check,EC20_INIT_STEP_GPRS_CHECK,"AT+CREG?\r\n","+CREG: ",10,5,5},                         /*10��GSM�źŲ�ѯ*/
       {"GPRS Check",EC20_GPRS_Check,EC20_INIT_STEP_SET_APN,"AT+CGREG?\r\n","+CGREG: ",10,5,5},                      /*11��GPRS�źŲ�ѯ*/
       {"GPRS APN set",EC20_GPRSAPN_Set,EC20_INIT_STEP_DEACT_PDP,"AT+QICSGP=1,1,\"CMNET\",\" \",\" \",0\r\n","OK\r\n",10,5,5},         /*12������GPRS��APN*/
       {"PDP Deactivate",EC20_PDP_Deactivate,EC20_INIT_STEP_ACT_PDP,"AT+QIDEACT=1\r\n","OK\r\n",20,200,5},               /*13��ȥ����PDP,���40s��Ӧ*/
       {"PDP Activate",EC20_PDP_Activate,EC20_INIT_STEP_OPEN_MSG,"AT+QIACT=1\r\n","OK\r\n",20,10,10},                  /*14������PDP*/

       /*15������һ��������*/
       {"Primary Server Open",EC20_Primary_Server_Open,EC20_INIT_STEP_OPEN_MSG,EC20_Init_Cmd[EC20_SERVER_PRI].open,EC20_Init_Cmd[EC20_SERVER_PRI].ack,20,50,10}, 
       /*16�����ӱ��ķ�����*/                   
       {"Message Server Open",EC20_Message_Server_Open,EC20_INIT_STEP_IDLE,EC20_Init_Cmd[EC20_SERVER_MSG].open,EC20_Init_Cmd[EC20_SERVER_MSG].ack,20,50,10}, 
       /*17������OTA������*/                     
       {"OTA Server Open",EC20_OTA_Server_Open,EC20_INIT_STEP_IDLE,EC20_Init_Cmd[EC20_SERVER_OTA].open,EC20_Init_Cmd[EC20_SERVER_OTA].ack,20,50,10}, 
       /*18������RTCM������*/  
       {"RTCM Server Open",EC20_RTCM_Server_Open,EC20_INIT_STEP_IDLE,EC20_Init_Cmd[EC20_SERVER_RTCM].open,EC20_Init_Cmd[EC20_SERVER_RTCM].ack,20,50,10}, 

	      /*19���ر�һ��������*/                       
       {"Primary Server Close",EC20_Primary_Server_Close,EC20_INIT_STEP_OPEN_PRI,EC20_Init_Cmd[EC20_SERVER_PRI].close,"OK\r\n",20,50,10},              
       /*20���رձ��ķ�����*/                   
       {"Message Server Close",EC20_Message_Server_Close,EC20_INIT_STEP_OPEN_MSG,EC20_Init_Cmd[EC20_SERVER_MSG].close,"OK\r\n",20,50,10},        
       /*21���رո��¹̼�������*/                  
       {"OTA Server Close",EC20_OTA_Server_Close,EC20_INIT_STEP_IDLE,EC20_Init_Cmd[EC20_SERVER_OTA].close,"OK\r\n",20,50,10},                           
			    /*22���ر�RTCM������*/ 
       {"RTCM Server Close",EC20_RTCM_Server_Close,EC20_INIT_STEP_OPEN_RTCM,EC20_Init_Cmd[EC20_SERVER_RTCM].close,"OK\r\n",20,50,10},

       {"EC20_Restart",EC20_Restart,EC20_INIT_STEP_BAT_ON,"AT+QPOWD=0\r\n","POWERED DOWN",20,50,10} ,                   /*23��ģ������*/
   }
};

static void EC20_Cmd_Set(EC20_INIT_STEP cmd_step,uint8_t repeat)
{
   EC20_dev.cmd_step=cmd_step;
   EC20_dev.times=EC20_dev.init[EC20_dev.cmd_step].times;
   if(repeat)
   {
       EC20_dev.repeat=EC20_dev.init[EC20_dev.cmd_step].repeat;
   }
}

static EC20_INIT_STEP EC20_Bat_On(void)
{
   TickType_t  delay_tick=pdMS_TO_TICKS(EC20_dev.init[EC20_dev.cur_step].delay);
   EC20_PWR_EN_ON();/*ģ���ϵ�*/
   EC20_PWR_HIGH();/*PWRKEY���ߴ���200ms*/
   
   FML_EC20_LOG("EC20 Init Info:BAT ON!");

   vTaskDelay(delay_tick);
   return EC20_dev.init[EC20_dev.cur_step].success_step;
}

void EC20_output_linkptr(EC20_SERVER server,Que_t *que)
{
   if (NULL==que)
   {
     return ;
   }
   EC20_output.device[server].que=que;
}

/**
* @brief  EC20ģ��ϵ�
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Bat_Off(void)
{
   TickType_t  delay_tick=pdMS_TO_TICKS(EC20_dev.init[EC20_dev.cur_step].delay);
   EC20_PWR_EN_OFF();
   EC20_PWR_LOW();
   EC20_PWR_HIGH();

   FML_EC20_LOG("EC20 Init Info:BAT OFF!");
   vTaskDelay(delay_tick);
   return EC20_dev.init[EC20_dev.cur_step].success_step;
}
/**
* @brief  EC20 PWRKEY����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_pwrkey_low(void)
{
   TickType_t  delay_tick=pdMS_TO_TICKS(EC20_dev.init[EC20_dev.cur_step].delay);

   EC20_PWR_LOW();/*PWRKEY����1S*/
   FML_EC20_LOG("EC20 Init Info:PWRKEY LOW!");

   vTaskDelay(delay_tick);
   
   return EC20_dev.init[EC20_dev.cur_step].success_step ;
}
/**
* @brief  EC20 PWRKEY����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_pwrkey_high(void)
{
   EC20_PWR_HIGH();/*PWRKEY����1s������*/
   HAL_GPIO_WritePin(EC20_WAKEUP_PORT,EC20_WAKEUP_PIN,GPIO_PIN_RESET);	
        
   FML_EC20_LOG("EC20 Init Info:PWRKEY HIGH!");

   EC20_Cmd_Set(EC20_INIT_STEP_SYNC,1);
   vTaskDelay(pdMS_TO_TICKS(15000));
   return EC20_dev.init[EC20_dev.cur_step].success_step ;
}

static EC20_INIT_STEP EC20_send_AT(void)
{ 
   EC20_INIT_STEP cmd_step=EC20_dev.cmd_step;
   TickType_t  delay_tick=pdMS_TO_TICKS(EC20_dev.init[EC20_dev.cur_step].delay);
   EC20_Send_cmd(EC20_dev.init[EC20_dev.cmd_step].cmd);
   vTaskDelay(delay_tick);
   return cmd_step;
}

/**
* @brief  EC20����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Restart(void)
{
   TickType_t  delay_tick=0;
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {  
       cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
       EC20_Cmd_Set(cmd_step,1);
       EC20_dev.restart++;
       if(EC20_dev.restart <= 3)
	      {
           delay_tick=pdMS_TO_TICKS(1*60*1000);/*��1,2,3��,ÿ���1��������һ��*/
           FML_EC20_LOG("Restart the EC20 in 1 minute");
       }

       else if((EC20_dev.restart > 3)&&(EC20_dev.restart <= 6))
		     {
			        delay_tick=pdMS_TO_TICKS(10*60*1000);/*��4,5,6��,ÿ���10��������һ��*/
           FML_EC20_LOG("Restart the EC20 in 10 minute");
		     }
		        
       else
		     {
			        delay_tick=pdMS_TO_TICKS(30*60*1000);/*��������6������,ÿ���30��������һ��*/
           FML_EC20_LOG("Restart the EC20 in 30 minute");
		     }         
   	   
     step=EC20_dev.init[EC20_dev.cur_step].success_step;
     vTaskDelay(delay_tick);
   }

   return step;
}

uint16_t EC20_Receive_Date(uint32_t xTicksToWait)
{
   uint16_t len=0;
   Que_t *EC20_input=App_Mem_Get(EC20_INIT_INPUT);

   if(pdPASS == xSemaphoreTake(EC20_BinarySem_Handle,xTicksToWait))
   {
       memset(EC20_input_buf,EC20_IN_MAX,0);
       len=Que_data_read(EC20_input,EC20_input_buf);
       DeQueueEx(EC20_input,len);
   }
   return len;
}
static EC20_INIT_STEP EC20_Receive_Error_handle(void)
{
   const char *str=EC20_dev.init[EC20_dev.cur_step].name;
   TickType_t  delay=pdMS_TO_TICKS(EC20_dev.init[EC20_dev.cur_step].delay);

   if(EC20_dev.times)
   {
       EC20_dev.times --;
       USART3_printf("EC20 Init Info:%s --> wait......\r\n",str);
       vTaskDelay(delay);
       return EC20_dev.cur_step;
   }

   if(EC20_dev.repeat)
   {
       EC20_dev. repeat--;
       /*���·�ָ��*/
       USART3_printf("EC20 Init Info:%s --> repeat\r\n",str);
       EC20_Cmd_Set(EC20_dev.cur_step,0);
       vTaskDelay(delay);
       return EC20_INIT_STEP_SEND_CMD;
   }

   USART3_printf("EC20 Init Info:%s --> fail\r\n",str);
   EC20_Cmd_Set(EC20_INIT_STEP_RESTART,1);
   vTaskDelay(delay);
   /*�����豸*/
   return  EC20_INIT_STEP_SEND_CMD;
}

static EC20_INIT_STEP EC20_Generic_init(char **dest,uint32_t xTicksToWait)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   if(EC20_Receive_Date(xTicksToWait))
   {   
       *dest=strstr((const char *)EC20_input_buf,EC20_dev.init[EC20_dev.cur_step].ack);
       if(NULL != *dest)
	      {
           return EC20_INIT_STEP_SEND_CMD;
	      }	
   }

   step=EC20_Receive_Error_handle();

   return step;
}

static EC20_INIT_STEP EC20_AT_Sync_Check(void)   
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static EC20_INIT_STEP EC20_Echoes_Close_Check(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static EC20_INIT_STEP EC20_Baudrate_Set_Check(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
   }
   
   return step;
}

static EC20_INIT_STEP EC20_Sim_Check(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {  
     LED_EC20_OK();
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     EC20_dev.init_status.state.SIM=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.SIM=EC20_INIT_ERR;

   return step;
}
static EC20_INIT_STEP EC20_Signal_Quality_Handle(char *src)
{
   char *buf=NULL;
   uint32_t quality=0;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;

   strtok(src,": ");
		 buf=strtok(NULL,": ");
	
		 buf=strtok(buf,",");
	  quality=numstr_to_num((uint8_t*)buf,2);

   if((99==quality)||(199==quality))
   {
       LED_EC20_ERR();
       EC20_dev.init_status.state.SQ=EC20_INIT_ERR;
	    	 FML_EC20_LOG("EC20 Init Info:%s --> Not known or not detectable",str);

       step=EC20_Receive_Error_handle();
       return step;
   }

		 if(quality<=31)/*�ź�����*/
	  {	
			    LED_EC20_OK();
			    EC20_dev.init_status.state.SQ=EC20_INIT_OK;
	    	 FML_EC20_LOG("EC20 Init Info:%s --> PASS(-%d dBm)",str,113-(quality<<1));       
		 }
 
   if((99<quality)&&(quality<192))
   {
       LED_EC20_OK();
			    EC20_dev.init_status.state.SQ=EC20_INIT_OK;
	    	 FML_EC20_LOG("EC20 Init Info:%s --> PASS(-%d dBm)",str,216-quality);
   }  

   step=EC20_dev.init[EC20_dev.cur_step].success_step;
   EC20_Cmd_Set(step,1);
   return EC20_INIT_STEP_SEND_CMD;
}
/**
* @brief  ����ź�ǿ�Ⱥ�������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Signal_Check(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
       step=EC20_Signal_Quality_Handle(dest);
       if(EC20_INIT_STEP_SEND_CMD==step)
       {
           cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
           EC20_Cmd_Set(cmd_step,1);
       }
   }  
   return step;
}
/**
* @brief  ���GSM�����Ƿ�ע��
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Network_Status_Check(void)
{
   uint32_t status=0;
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
       strtok(dest,": ");
       dest=strtok(NULL,": ");
       strtok(dest,",");
       dest=strtok(NULL,",");
		     status=numstr_to_num((uint8_t*)dest,1);
		     if((0==status)||(2==status)||(4==status))
		     {
			        LED_EC20_ERR();
           EC20_dev.init_status.state.GSM=EC20_INIT_ERR;
			        FML_EC20_LOG("EC20 Init Info:%s --> NO registe",str);
           EC20_Cmd_Set(EC20_dev.cur_step,0);
           return EC20_INIT_STEP_SEND_CMD;
	  	   }
       LED_EC20_OK();
       EC20_dev.init_status.state.GSM=EC20_INIT_OK;
       cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
       EC20_Cmd_Set(cmd_step,1);
			    FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
   }
   return step;
}
/**
* @brief  ���GPRS�����Ƿ�ע��
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_GPRS_Check(void)
{
   uint32_t status=0;
	  EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;
   char *dest=NULL;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
		     strtok(dest,": ");
      	dest=strtok(NULL,": ");
		     strtok(dest,",");
		     dest=strtok(NULL,",");
		     status=numstr_to_num((uint8_t*)dest,1);
	      if((0==status)||(2==status)||(4==status))
		     {
			        LED_EC20_ERR();
           EC20_dev.init_status.state.GPRS=EC20_INIT_ERR;
			        FML_EC20_LOG("EC20 Init Info:%s --> NO registe",str);
           EC20_Cmd_Set(EC20_dev.cur_step,0);
           return EC20_INIT_STEP_SEND_CMD;
		     }
		     LED_EC20_OK();
		     EC20_dev.init_status.state.GPRS=EC20_INIT_OK;
       cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
       EC20_Cmd_Set(cmd_step,1);
		     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
	  }
   return step;
}
/**
* @brief  ����GPRS��APN���ƶ�CMNET����ͨUNINET
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_GPRSAPN_Set(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.APN=EC20_INIT_OK;
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.APN=EC20_INIT_ERR;
   return step;
}
/**
* @brief  ͣ���ض������Ĳ��ر��ڴ������������õ�����TCP / IP����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_PDP_Deactivate(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.DEACTPDP=EC20_INIT_OK;
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.DEACTPDP=EC20_INIT_ERR;
   return step;
}

/**
* @brief  ����PDP������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_PDP_Activate(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.ACTPDP=EC20_INIT_OK;
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.ACTPDP=EC20_INIT_ERR;
   return step;
}

/**
* @brief  ����һ��������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Primary_Server_Open(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {

     LED_EC20_OK();
     EC20_dev.init_status.state.PRI=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.PRI=EC20_INIT_ERR;
   return step;
}

/**
* @brief  �ر�һ��������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Primary_Server_Close(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {

     LED_EC20_OK();
     EC20_dev.init_status.state.PRI=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.PRI=EC20_INIT_ERR;
   return step;
}

/**
* @brief  ���ӱ��ķ����������ڷ��ͱ��ĵ�������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Message_Server_Open(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.MSG=EC20_INIT_OK;
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     vTaskResume(SDM_OUTPUT_Task_Handle);

     xTaskNotify((TaskHandle_t	)SDM_OUTPUT_Task_Handle,(uint32_t		)MSG_INITED_EVENT,	(eNotifyAction)eSetBits);
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.MSG=EC20_INIT_ERR;
   return step;
}

/**
* @brief  �رձ��ķ�����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_Message_Server_Close(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.MSG=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.MSG=EC20_INIT_ERR;
   return step;
}

/**
* @brief  ����OTA�����������ڹ̼�����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_OTA_Server_Open(void)
{ 
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.OTA=EC20_INIT_OK;
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.OTA=EC20_INIT_ERR;
   return step;
}
/**
* @brief  �رչ̼����·�����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_OTA_Server_Close(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.OTA=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.OTA=EC20_INIT_ERR;
   return step;
}
/**
* @brief  ����RTCM������
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_RTCM_Server_Open(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE,cmd_step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.RTCM=EC20_INIT_OK;
     
     cmd_step=EC20_dev.init[EC20_dev.cur_step].success_step;
     EC20_Cmd_Set(cmd_step,1);
     vTaskResume(RTCM_OUTPUT_Task_Handle);
     xTaskNotify((TaskHandle_t	)RTCM_OUTPUT_Task_Handle,(uint32_t		)RTCM_INITED_EVENT,	(eNotifyAction)eSetBits);
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.RTCM=EC20_INIT_ERR;
   return step;

}
/**
* @brief  �رշ�����
* @param  none
* @retval none
*/
static EC20_INIT_STEP EC20_RTCM_Server_Close(void)
{
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;
   char *dest=NULL;
   char *str=EC20_dev.init[EC20_dev.cur_step].name;

   step=EC20_Generic_init(&dest,pdMS_TO_TICKS(10));

   if(NULL != dest)
   {
     LED_EC20_OK();
     EC20_dev.init_status.state.RTCM=EC20_INIT_OK;
     FML_EC20_LOG("EC20 Init Info:%s --> PASS",str);
     return step;
   }
   LED_EC20_ERR();
   EC20_dev.init_status.state.RTCM=EC20_INIT_ERR;
   return step;
}

/**
* @brief  ��ȡEC20������״̬
* @param  none
* @retval 0x00:EC20���ӷ�����ʧ��,0x01:EC20���ӷ������ɹ�
*/
/*uint16_t EC20_Init_State_Get(EC20_SERVER_t s)
{
	  uint16_t state=0;

   switch(s)
   {
       case SERVER_PRI:
       {state=SERVER_PRI_Msk&EC20_dev.init_status.status;}break;

       case SERVER_MSG:
       {state=SERVER_MSG_Msk&EC20_dev.init_status.status;}break;

       case SERVER_UPDATE:
       {state=SERVER_UPDATE_Msk&EC20_dev.init_status.status;}break;

       case SERVER_RTK:
       {state=SERVER_RTK_Msk&EC20_dev.init_status.status;}break;

       case SERVER_ALL:
       {state=SERVER_ALL_Msk&EC20_dev.init_status.status;}break;
   } 
   return state;
}*/
uint8_t EC20_MSG_Server_Open(void)
{
   return (EC20_dev.init_status.state.MSG==EC20_INIT_OK);
}

void EC20_Init_Cmd_Set(Server_Config_t *config)
{
   sprintf(EC20_Init_Cmd[config->id].open,"AT+QIOPEN=1,%d,\"TCP\",\"%s\",%d,0,1\r\n",config->id,config->ip,config->port);
   sprintf(EC20_Init_Cmd[config->id].ack,"+QIOPEN: %d,0\r\n",config->id);
   sprintf(EC20_Init_Cmd[config->id].close,"AT+QICLOSE=%d\r\n",config->id);
}

void EC20_Init_Config_Get(void)
{
   Server_Config_t *config=NULL;
   uint16_t offset=0,loop=0;
   EC20_INIT_STEP step=EC20_INIT_STEP_IDLE;

   config=&EC20_output.device[EC20_SERVER_PRI].config;
   /*primary server ip and port*/
   if(I2c_Server_Config_Get(config))
   {
      EC20_Init_Cmd_Set(config);
      EC20_dev.connect |= SERVER_PRI_Msk;
   }

   config=&EC20_output.device[EC20_SERVER_MSG].config;
   /*message server ip and port*/
   if(I2c_Server_Config_Get(config))
   {
      EC20_Init_Cmd_Set(config);
      EC20_dev.connect |= SERVER_MSG_Msk;
   }

   config=&EC20_output.device[EC20_SERVER_OTA].config;
   /*OTA server ip and port*/
   if(I2c_Server_Config_Get(config))
   {
      EC20_Init_Cmd_Set(config);
      EC20_dev.connect |= SERVER_UPDATE_Msk;
   }

   config=&EC20_output.device[EC20_SERVER_RTCM].config;
   /*RTCM server ip and port*/
   if(I2c_Server_Config_Get(config))
   {
      EC20_Init_Cmd_Set(config);
      EC20_dev.connect |= SERVER_RTCM_Msk;
   }

   for(offset=SERVER_PRI_Pos;offset<=SERVER_RTCM_Pos;offset++)
   {
       if(EC20_dev.connect & (1<<offset))
       {
         loop ++;
       }
   }

   for(offset=SERVER_PRI_Pos;offset<=SERVER_RTCM_Pos;offset++)
   {
       if(EC20_dev.connect & (1<<offset))
       {
           EC20_dev.init[EC20_INIT_STEP_ACT_PDP].success_step=(EC20_INIT_STEP)(0x08+offset);   
           break;  
       }
   }
 
   if(offset>SERVER_RTCM_Pos)
   {
       EC20_dev.init[EC20_INIT_STEP_ACT_PDP].success_step=EC20_INIT_STEP_IDLE;
       return ;
   }

   loop --;

   while(loop)
   {
       step=(EC20_INIT_STEP)(0x08+offset);
       for(offset++;offset<=SERVER_RTCM_Pos;offset++)
       {
           if(EC20_dev.connect & (1<<offset))
           {
               EC20_dev.init[step].success_step=(EC20_INIT_STEP)(0x08+offset);   
               loop --;
               break;  
           }
       }
   }

   EC20_dev.init[(0x08+offset)].success_step= EC20_INIT_STEP_IDLE;
}
void EC20_Init_Task(void *argv)
{
   while(1)
   {
       EC20_dev.cur_step=EC20_dev.init[EC20_dev.cur_step].act();/*connect server*/

       if(EC20_INIT_STEP_IDLE == EC20_dev.cur_step)
       {
           USART3_printf("EC20 Init Task Suspend\r\n");
           vTaskSuspend(NULL);
       }
   }
}


void EC20_SDM_Output_Task(void *argv);
void EC20_RTCM_Output_Task(void *argv);

void EC20_Task_Create(void)
{
   BaseType_t xReturn = pdPASS;

   EC20_Init_Config_Get();

   if(0==EC20_dev.connect)
   {
       return ;
   }

   bsp_EC20_Hardware_init();

   EC20_BinarySem_Handle=xSemaphoreCreateBinary();
   if(NULL == EC20_BinarySem_Handle)
   {
       USART3_printf("EC20 BinarySem create fail!\n");
       return ;
   }

   xReturn = xTaskCreate((TaskFunction_t )EC20_Init_Task,  
                         (const char*    )EC20_INIT_TASK_NAME,
                         (uint16_t       )EC20_INIT_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )EC20_INIT_TASK_PRIO, 
                         (TaskHandle_t*  )&EC20_INIT_Task_Handle);
  if(pdFAIL == xReturn)
  {
    USART3_printf("EC20 Init Task create fail!\n");
    return;
  }  


   xReturn = xTaskCreate((TaskFunction_t )EC20_SDM_Output_Task,  
                         (const char*    )EC20_MAIN_TASK_NAME,
                         (uint16_t       )EC20_MAIN_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )EC20_MAIN_TASK_PRIO, 
                         (TaskHandle_t*  )&SDM_OUTPUT_Task_Handle);
   if(pdFAIL == xReturn)
   {
       USART3_printf("EC20 Main Task create fail!\n");
       return;
   } 

   xReturn = xTaskCreate((TaskFunction_t )EC20_RTCM_Output_Task,  
                         (const char*    )EC20_MAIN_TASK_NAME,
                         (uint16_t       )EC20_MAIN_TASK_STACK_SIZE,  
                         (void*          )NULL,
                         (UBaseType_t    )EC20_MAIN_TASK_PRIO, 
                         (TaskHandle_t*  )&RTCM_OUTPUT_Task_Handle);
   if(pdFAIL == xReturn)
   {
       USART3_printf("EC20 Main Task create fail!\n");
       return;
   } 

   EC20_MuxSem_Handle=xSemaphoreCreateMutex();
   if(NULL ==EC20_MuxSem_Handle)
   {
       USART3_printf("EC20 Mutex create fail!\n");
       return;
   }
   xSemaphoreGive(EC20_MuxSem_Handle);
   vTaskSuspend(SDM_OUTPUT_Task_Handle);
   vTaskSuspend(RTCM_OUTPUT_Task_Handle);
}

EC20_OUTPUT_STEP EC20_Output_Cmd(void)
{
   char cmd[20]={0};
   uint16_t send_len=0;

   EC20_output.ack =0;
   send_len=GetQueLength(EC20_output.pdev->que);
   EC20_output.pdev->send_len= send_len > 1024 ? 1024 : send_len ;
   sprintf(cmd,"AT+QISEND=%d,%d\r\n",EC20_output.pdev->config.id,EC20_output.pdev->send_len);
	  EC20_Send_cmd(cmd);
   vTaskDelay(10);
   return EC20_OUTPUT_STEP_WAIT_ACK;
}
void EC20_RTCM_Output_Task(void *argv)
{
   uint32_t r_event = 0;  
   BaseType_t xReturn = pdPASS;
   static  EC20_OUTPUT_STEP step=EC20_OUTPUT_STEP_START;

   while(1)
   {         
       switch(step)
       {
           case   EC20_OUTPUT_STEP_START:
           {
               xReturn = xTaskNotifyWait(0x0,0xfffffffe,&r_event,portMAX_DELAY);	
               printf("r_event:%d\r\n",r_event);
               if( pdTRUE == xReturn )
               {   
                   xReturn=xSemaphoreTake(EC20_MuxSem_Handle,portMAX_DELAY);
                   if(pdPASS==xReturn)
                   {
                      EC20_output.pdev=&EC20_output.device[EC20_SERVER_RTCM];
                      step=EC20_Output_Cmd();
                   }
               }
           }break;

           case EC20_OUTPUT_STEP_WAIT_ACK:
           {
               step=EC20_output_wait_ack();
           }break;
       }
   }

}

void EC20_SDM_Output_Task(void *argv)
{
   uint32_t r_event = 0;  
   BaseType_t xReturn = pdPASS;
   static  EC20_OUTPUT_STEP step=EC20_OUTPUT_STEP_START;

   while(1)
   {         
       switch(step)
       {
           case   EC20_OUTPUT_STEP_START:
           {
               xReturn = xTaskNotifyWait(0x0,0xfffffffe,&r_event,portMAX_DELAY);	
               if( pdTRUE == xReturn )
               {   
                   if((r_event&(MSG_INITED_EVENT|SDM_SEND_EVENT)) ==(MSG_INITED_EVENT|SDM_SEND_EVENT))
                   {
                       xReturn=xSemaphoreTake(EC20_MuxSem_Handle,portMAX_DELAY);
                       if(pdPASS==xReturn)
                       {
                           EC20_output.pdev=&EC20_output.device[EC20_SERVER_MSG];
                           step=EC20_Output_Cmd();
                       }
                   }
               }
           }break;

           case EC20_OUTPUT_STEP_WAIT_ACK:
           {
               step=EC20_output_wait_ack();
           }break;
       }
   }
}

void EC20_ack_check(uint8_t *pbuf,uint16_t len)
{
   uint16_t i=0;
   uint8_t *ptemp=pbuf;

   /*check EC20'S response string ">"*/
   for(i=0;i<len;i++)
   {
       if(*ptemp++=='>')
       {
           EC20_output.ack |=1;
       }
    }
}

void Queue_Remove_Handle(Que_t *que, uint16_t size) 
{
   /*�ж����з�������ʼ���Ƿ���ɣ���ɵĻ����ӣ����򲻳���*/
   if(EC20_dev.connect ==(EC20_dev.init_status.status&0x0780))
   {
       DeQueueEx(que,size);
   }
}

void EC20_output_start_ack_check(void)
{
   TickType_t  xTicksToWait=pdMS_TO_TICKS(10);
   uint16_t len=0;
   Que_t *EC20_input=App_Mem_Get(EC20_INIT_INPUT);

   if(EC20_output.ack)
   {
       return ;
   }

   if(pdPASS == xSemaphoreTake(EC20_BinarySem_Handle,xTicksToWait))
   {
       memset(EC20_input_buf,EC20_IN_MAX,0);
       len=Que_data_read(EC20_input,EC20_input_buf);
       Queue_Remove_Handle(EC20_input,len);
   }

   if(len)
   {   
       if(NULL != strstr((const char *)EC20_input_buf,">"))
	      {
           EC20_output.ack |=1;
           return ;
	      }	
   }
   EC20_output.ack |=0;
}

EC20_OUTPUT_STEP EC20_output_wait_ack(void)
{
   uint16_t len=EC20_output.pdev->send_len;
   EC20_output_start_ack_check();
   
   if(EC20_output.ack)
   {          
   	   memset(EC20_out_buf,0,EC20_OUT_MAX);
			    CpyQueData(EC20_output.pdev->que,EC20_out_buf,len);
       DeQueueEx(EC20_output.pdev->que,len);
       USART2_data_send(EC20_out_buf,len);
       xSemaphoreGive(EC20_MuxSem_Handle);
       return EC20_OUTPUT_STEP_START;
       //return EC20_OUTPUT_STEP_QUERY;
   }

   if(EC20_output.wait)
   {
      EC20_output.wait --;
      vTaskDelay(pdMS_TO_TICKS(10));
      return EC20_OUTPUT_STEP_WAIT_ACK;
   }

   if(EC20_output.repeat)
   {
       EC20_output.wait=10;
       return  EC20_Output_Cmd();
   }

   EC20_output.wait=10;
   EC20_output.repeat=3;
   return EC20_OUTPUT_STEP_START;
}


EC20_OUTPUT_STEP EC20_output_query(void)
{
  return EC20_OUTPUT_STEP_START;
}



uint8_t EC20_output_idle(EC20_output_t *output)
{
  return 0;
}

uint8_t Is_RTK_server_connected(void)
{
	return 0;
}
