#include "bsp_usart6.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static UART_HandleTypeDef huart6;
static DMA_HandleTypeDef hdma_usart6_tx;
static DMA_HandleTypeDef hdma_usart6_rx;

__align(4) uint8_t usart6_tx[USART6_TX_MAX]={0};
static uint8_t usart6_rx[USART6_RX_MAX]={0};

SemaphoreHandle_t USART6_MuxSem_Handle=NULL;



static void bsp_usart6_config(void)
{
   /* USART6 clock enable */
  USART6_CLK_ENABLE();

  huart6.Instance = USART6;
  huart6.Init.BaudRate = USART6_BAUDRATE;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;

#if defined(USE_USART6_RX) && !defined(USE_USART6_TX)
  huart6.Init.Mode = UART_MODE_RX;
#endif

#if defined(USE_USART6_TX) && !defined(USE_USART6_RX)
  huart6.Init.Mode = UART_MODE_TX;
#endif

#if defined(USE_USART6_RX) && defined(USE_USART6_TX)
  huart6.Init.Mode = UART_MODE_TX_RX;
#endif

  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(USART6_IRQn, USART6_MAINPRI,USART6_SUBPRI);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
}

static void bsp_usart6_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
 
#if defined(USE_USART6_RX)|| defined(USE_USART6_TX)

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART6_AF;
#endif

#ifdef USE_USART6_RX
    USART6_RX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART6_RX_PIN;
    HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_USART6_TX
    USART6_TX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART6_TX_PIN;
    HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

void bsp_usart6_gpio_Deinit(void)
{
    __HAL_RCC_USART6_CLK_DISABLE();

#ifdef USE_USART6_RX
    HAL_GPIO_DeInit(USART6_RX_GPIO_PORT, USART6_RX_PIN);
    HAL_DMA_DeInit(huart6.hdmarx);
#endif

#ifdef USE_USART6_TX
 HAL_GPIO_DeInit(USART6_TX_GPIO_PORT, USART6_TX_PIN);
 HAL_DMA_DeInit(huart6.hdmatx);
#endif
 /* USART1 interrupt Deinit */
 HAL_NVIC_DisableIRQ(USART6_IRQn);
}

static void bsp_usart6_DMA_cfg(void)
{
   /* DMA controller clock enable */
				__HAL_RCC_DMA2_CLK_ENABLE();

#ifdef USE_USART6_TX_DMA
    /* USART1_RX Init */
    hdma_usart6_tx.Instance = USART6_TX_DMA_STREAM;
    hdma_usart6_tx.Init.Channel = USART6_TX_DMA_CHANNEL;
    hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart6_tx.Init.Mode = DMA_NORMAL;
    hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart6,hdmatx,hdma_usart6_tx);

				HAL_NVIC_SetPriority(USART6_TX_DMA_IRQn, 7, 0);
				HAL_NVIC_EnableIRQ(USART6_TX_DMA_IRQn);

#endif

#ifdef USE_USART6_RX
    hdma_usart6_rx.Instance = USART6_RX_DMA_STREAM;
    hdma_usart6_rx.Init.Channel = USART6_RX_DMA_CHANNEL;
    hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart6_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart6_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart6_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart6,hdmarx,hdma_usart6_rx);

				/* DMA2_Stream7_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(USART6_RX_DMA_IRQn, 7, 0);
				HAL_NVIC_EnableIRQ(USART6_RX_DMA_IRQn);
#endif
}

void bsp_USART6_init(void)
{
  bsp_usart6_gpio_init();
  bsp_usart6_DMA_cfg();
  bsp_usart6_config();

  HAL_UART_Receive_DMA(&huart6,usart6_rx,USART6_RX_MAX);
  __HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);

	__HAL_UART_CLEAR_FLAG(&huart6,UART_FLAG_TC);

 USART6_MuxSem_Handle=xSemaphoreCreateMutex();
}

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);

  if(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_IDLE)!=RESET)
 {
   __HAL_UART_CLEAR_IDLEFLAG(&huart6);
   USART6_Receive_Set();
 }
}


#ifdef USE_USART6_RX

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void USART6_RX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_rx);
}



USART_cnt_t USART6_cnt={0,0};

uint16_t USART6_Data_Receive(uint8_t *dest)
{
   uint16_t len=0;
   USART6_cnt.cur_cnt=USART6_RX_MAX - (uint16_t)(USART6_RX_DMA_STREAM->NDTR);

   if(USART6_cnt.cur_cnt == USART6_cnt.old_cnt)
   {
       return 0;
   }

   if(USART6_cnt.cur_cnt > USART6_cnt.old_cnt)
   {
      len=USART6_cnt.cur_cnt - USART6_cnt.old_cnt;
      memcpy(dest,usart6_rx + USART6_cnt.old_cnt,len);
   }
   else
   {
       len=USART2_RX_MAX - USART6_cnt.old_cnt;
       memcpy(dest,usart6_rx + USART6_cnt.old_cnt,len);
       memcpy(dest+len,usart6_rx,USART6_cnt.cur_cnt);
       len += USART6_cnt.cur_cnt;
   }
    
   USART6_cnt.old_cnt=USART6_cnt.cur_cnt;
   return len;
}

#endif

static void log_printf_mutex_lock(void)
{

    if ( __get_IPSR() ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(USART6_MuxSem_Handle,portMAX_DELAY);
}

static void log_printf_mutex_unlock(void)
{
    if ( __get_IPSR() || (!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(USART6_MuxSem_Handle);
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART6_data_send(uint8_t *pData, uint16_t Size)
{
   log_printf_mutex_lock();
   HAL_UART_Transmit(&huart6, pData, Size, 1000);
   log_printf_mutex_unlock();
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 1000);		
	return (ch);
}

void USART6_printf(char* fmt,...)  
{  
	uint16_t slen;
	va_list ap;
 log_printf_mutex_lock();
	va_start(ap,fmt);
	vsprintf((char*)usart6_tx,fmt,ap);
	va_end(ap);
	slen=strlen((const char*)usart6_tx);//此次发送数据的长度

 HAL_UART_Transmit(&huart6, usart6_tx, slen, 1000);
 log_printf_mutex_unlock();
}
void USART6_Log(char* fmt,...)  
{  
	  uint16_t slen;
	  va_list ap;
   log_printf_mutex_lock();
	  va_start(ap,fmt);
	  vsprintf((char*)usart6_tx,fmt,ap);
	  va_end(ap);
	  slen=strlen((const char*)usart6_tx);//此次发送数据的长度
   usart6_tx[slen]='\r';
   usart6_tx[slen+1]='\n';

   HAL_UART_Transmit(&huart6, usart6_tx, slen+2, 1000);
   log_printf_mutex_unlock();
}

#ifdef USE_USART6_TX_DMA
void USART6_DMA_printf(char* fmt,...)  
{  
	  uint16_t slen;
	  va_list ap;

   log_printf_mutex_lock();
	  va_start(ap,fmt);
	  vsprintf((char*)usart6_tx,fmt,ap);
	  va_end(ap);
	  slen=strlen((const char*)usart6_tx);//此次发送数据的长度

   HAL_UART_Transmit_DMA(&huart6, usart6_tx, slen);
   log_printf_mutex_unlock();   
}

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART6_data_DMA_send(uint8_t *pData, uint16_t Size)
{
   log_printf_mutex_lock();
   HAL_UART_Transmit_DMA(&huart6, pData, Size);

  /*##Wait for the end of the transfer ###################################*/  
  while (HAL_UART_GetState(&huart6) != HAL_UART_STATE_READY)
  {
  } 

   log_printf_mutex_unlock();
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void USART6_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_tx);
}

#endif
