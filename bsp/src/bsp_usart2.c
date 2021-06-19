#include "bsp_usart2.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static UART_HandleTypeDef huart2;
static DMA_HandleTypeDef hdma_usart2_tx;
static DMA_HandleTypeDef hdma_usart2_rx;

static uint8_t usart2_rx[USART2_RX_MAX]={0};

static SemaphoreHandle_t USART2_MuxSem_Handle=NULL;

void USART2_Analog_Mode_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
 
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = USART2_RX_PIN;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);
}

static void bsp_usart2_config(void)
{
  USART2_CLK_ENABLE();

  huart2.Instance = USART2;
  huart2.Init.BaudRate = USART2_BAUDRATE;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

#if defined(USE_USART2_RX) && !defined(USE_USART2_TX)
  huart2.Init.Mode = UART_MODE_RX;
#endif

#if defined(USE_USART2_TX) && !defined(USE_USART2_RX)
  huart2.Init.Mode = UART_MODE_TX;
#endif

#if defined(USE_USART2_RX) && defined(USE_USART2_TX)
  huart2.Init.Mode = UART_MODE_TX_RX;
#endif

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(USART2_IRQn, USART2_MAINPRI,USART2_SUBPRI);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

static void bsp_usart2_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
 
#if defined(USE_USART2_RX)|| defined(USE_USART2_TX)

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART2_AF;
#endif

#ifdef USE_USART2_RX
    USART2_RX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART2_RX_PIN;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_USART2_TX
    USART2_TX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART2_TX_PIN;
    HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

void bsp_usart2_gpio_Deinit(void)
{
    __HAL_RCC_USART2_CLK_DISABLE();

#ifdef USE_USART2_RX
    HAL_GPIO_DeInit(USART2_RX_GPIO_PORT, USART2_RX_PIN);
    HAL_DMA_DeInit(huart2.hdmarx);
#endif

#ifdef USE_USART2_TX
 HAL_GPIO_DeInit(USART2_TX_GPIO_PORT, USART2_TX_PIN);
 HAL_DMA_DeInit(huart2.hdmatx);
#endif
 /* USART1 interrupt Deinit */
 HAL_NVIC_DisableIRQ(USART2_IRQn);
}

static void bsp_usart2_DMA_cfg(void)
{
   /* DMA controller clock enable */
				__HAL_RCC_DMA1_CLK_ENABLE();

    /* USART1_RX Init */
    hdma_usart2_tx.Instance = USART2_TX_DMA_STREAM;
    hdma_usart2_tx.Init.Channel = USART2_TX_DMA_CHANNEL;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart2,hdmatx,hdma_usart2_tx);

				HAL_NVIC_SetPriority(USART2_TX_DMA_IRQn, 2, 2);
				HAL_NVIC_EnableIRQ(USART2_TX_DMA_IRQn);


    hdma_usart2_rx.Instance = USART2_RX_DMA_STREAM;
    hdma_usart2_rx.Init.Channel = USART2_RX_DMA_CHANNEL;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart2,hdmarx,hdma_usart2_rx);

				/* DMA2_Stream7_IRQn interrupt configuration */
				HAL_NVIC_SetPriority(USART2_RX_DMA_IRQn, 2, 1);
				HAL_NVIC_EnableIRQ(USART2_RX_DMA_IRQn);
}

void bsp_USART2_init(void)
{
   bsp_usart2_gpio_init();
   bsp_usart2_DMA_cfg();
   bsp_usart2_config();

   HAL_UART_Receive_DMA(&huart2,usart2_rx,USART2_RX_MAX);
   __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);

	  __HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_TC);

   USART2_MuxSem_Handle=xSemaphoreCreateMutex();
}

void bsp_USART2_DeInit(void)
{
  HAL_UART_DeInit(&huart2);
  USART2_Analog_Mode_cfg();
}


void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);

  if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE)!=RESET)
 {
   __HAL_UART_CLEAR_IDLEFLAG(&huart2);
   AIR724_Receive_Set();
 }
}
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void USART2_RX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
}
/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void USART2_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
}


USART_cnt_t USART2_cnt={0,0};

uint16_t USART2_Cnt_Len(void)
{
   uint16_t len=0;
   USART2_cnt.cur_cnt=USART2_RX_MAX - (uint16_t)(USART2_RX_DMA_STREAM->NDTR);

   if(USART2_cnt.cur_cnt == USART2_cnt.old_cnt)
   {
       return 0;
   }

   if(USART2_cnt.cur_cnt > USART2_cnt.old_cnt)
   {
      len=USART2_cnt.cur_cnt - USART2_cnt.old_cnt;
   }
   else
   {
       len=USART2_RX_MAX - USART2_cnt.old_cnt + USART2_cnt.cur_cnt;
   }
   return len;
}

void USART2_Cnt_Get(void)
{
   USART2_cnt.cur_cnt=USART2_RX_MAX - (uint16_t)(USART2_RX_DMA_STREAM->NDTR);
}

uint8_t Is_USART2_Received(void)
{
   uint16_t len=USART2_RX_MAX - (uint16_t)(USART2_RX_DMA_STREAM->NDTR);
   return (len != USART2_cnt.old_cnt);
}

void USART2_Cnt_Flush(void)
{
   USART2_cnt.old_cnt=USART2_cnt.cur_cnt;
}

uint16_t USART2_Data_EnQueue(Que_t *que)
{
   if(USART2_cnt.cur_cnt == USART2_cnt.old_cnt)
   {
       return 0;
   }

   if(USART2_cnt.cur_cnt > USART2_cnt.old_cnt)
   {
      EnQueueEx(que,usart2_rx+USART2_cnt.old_cnt,USART2_cnt.cur_cnt - USART2_cnt.old_cnt);
   }
   else
   {
       EnQueueEx(que,usart2_rx+USART2_cnt.old_cnt,USART2_RX_MAX-USART2_cnt.old_cnt);
       EnQueueEx(que,usart2_rx,USART2_cnt.cur_cnt);
   }

   return que->len;
}

uint16_t USART2_Data_Copy(uint8_t *pbuf)
{
   if(USART2_cnt.cur_cnt == USART2_cnt.old_cnt)
   {
       return 0;
   }

   if(USART2_cnt.cur_cnt > USART2_cnt.old_cnt)
   {
      memcpy(pbuf,usart2_rx+USART2_cnt.old_cnt,USART2_cnt.cur_cnt - USART2_cnt.old_cnt);
      return USART2_cnt.cur_cnt - USART2_cnt.old_cnt;
   }

   memcpy(pbuf,usart2_rx+USART2_cnt.old_cnt,USART2_RX_MAX-USART2_cnt.old_cnt);
   memcpy(pbuf,usart2_rx,USART2_cnt.cur_cnt);
   
   return (USART2_RX_MAX-USART2_cnt.old_cnt+USART2_cnt.cur_cnt);
}

void USART2_Data_Printf(void)
{
   if(USART2_cnt.cur_cnt == USART2_cnt.old_cnt)
   {
       return ;
   }

   if(USART2_cnt.cur_cnt > USART2_cnt.old_cnt)
   {
      USART6_data_send(usart2_rx+USART2_cnt.old_cnt,USART2_cnt.cur_cnt - USART2_cnt.old_cnt);
   }
   else
   {
       USART6_data_send(usart2_rx+USART2_cnt.old_cnt,USART2_RX_MAX - USART2_cnt.old_cnt);
       USART6_data_send(usart2_rx,USART2_cnt.cur_cnt);
   }
}

static void usart2_printf_mutex_lock(void)
{

    if ( __get_IPSR() ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(USART2_MuxSem_Handle,portMAX_DELAY);
}

static void usart2_printf_mutex_unlock(void)
{
    if ( __get_IPSR() || (!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(USART2_MuxSem_Handle);
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART2_data_send(uint8_t *pData, uint16_t Size)
{
   usart2_printf_mutex_lock();
   HAL_UART_Transmit(&huart2, pData, Size, 1000);
   usart2_printf_mutex_unlock();
}

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART2_data_DMA_send(uint8_t *pData, uint16_t Size)
{
   usart2_printf_mutex_lock();
   HAL_UART_Transmit_DMA(&huart2, pData, Size);
   usart2_printf_mutex_unlock();
}
/**
* @brief  EC20发送字符串
* @param  str：待发送的数据
* @retval none
*/
void EC20_Send_cmd(void *pstr)
{
	uint16_t i=0;
	uint8_t *ptemp=(uint8_t *)pstr;
 usart2_printf_mutex_lock();
	do 
	{
   HAL_UART_Transmit(&huart2, (ptemp + i), 1, 1000);
			i++;
	} while(*(ptemp + i)!='\0');
usart2_printf_mutex_unlock();
}

