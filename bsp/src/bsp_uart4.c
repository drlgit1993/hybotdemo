#include "bsp_uart4.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static UART_HandleTypeDef huart4;
static DMA_HandleTypeDef  hdma_uart4_tx;
static DMA_HandleTypeDef  hdma_uart4_rx;

static uint8_t uart4_rx[UART4_RX_MAX]={0};
__align(4) uint8_t uart4_tx[128]={0};

static SemaphoreHandle_t UART4_MuxSem_Handle=NULL;

static void bsp_uart4_config(void)
{
  UART4_CLK_ENABLE();

  huart4.Instance = UART4;
  huart4.Init.BaudRate = UART4_BAUDRATE;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;

#if defined(USE_UART4_RX) && !defined(USE_UART4_TX)
  huart4.Init.Mode = UART_MODE_RX;
#endif

#if defined(USE_UART4_TX) && !defined(USE_UART4_RX)
  huart4.Init.Mode = UART_MODE_TX;
#endif

#if defined(USE_UART4_RX) && defined(USE_UART4_TX)
  huart4.Init.Mode = UART_MODE_TX_RX;
#endif

  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(UART4_IRQn, UART4_MAINPRI,UART4_SUBPRI);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

static void bsp_uart4_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
 
#if defined(USE_UART4_RX)|| defined(USE_UART4_TX)

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART4_AF;
#endif

#ifdef USE_UART4_RX
    UART4_RX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = UART4_RX_PIN;
    HAL_GPIO_Init(UART4_RX_GPIO_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_UART4_TX
    UART4_TX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = UART4_TX_PIN;
    HAL_GPIO_Init(UART4_TX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

void bsp_uart4_gpio_Deinit(void)
{
    __HAL_RCC_UART4_CLK_DISABLE();

#ifdef USE_UART4_RX
    HAL_GPIO_DeInit(UART4_RX_GPIO_PORT, UART4_RX_PIN);
    HAL_DMA_DeInit(huart4.hdmarx);
#endif

#ifdef USE_UART4_TX
 HAL_GPIO_DeInit(UART4_TX_GPIO_PORT, UART4_TX_PIN);
 HAL_DMA_DeInit(huart4.hdmatx);
#endif
 /* USART1 interrupt Deinit */
 HAL_NVIC_DisableIRQ(UART4_IRQn);
}

static void bsp_uart4_DMA_cfg(void)
{
   /* DMA controller clock enable */
				__HAL_RCC_DMA1_CLK_ENABLE();

    /* USART1_RX Init */
    hdma_uart4_tx.Instance = UART4_TX_DMA_STREAM;
    hdma_uart4_tx.Init.Channel = UART4_TX_DMA_CHANNEL;
    hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_tx.Init.Mode = DMA_NORMAL;
    hdma_uart4_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart4,hdmatx,hdma_uart4_tx);

				HAL_NVIC_SetPriority(UART4_TX_DMA_IRQn, 2, 4);
				HAL_NVIC_EnableIRQ(UART4_TX_DMA_IRQn);

#ifdef USE_UART4_RX
    hdma_uart4_rx.Instance = UART4_RX_DMA_STREAM;
    hdma_uart4_rx.Init.Channel = UART4_RX_DMA_CHANNEL;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_CIRCULAR;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart4,hdmarx,hdma_uart4_rx);

				HAL_NVIC_SetPriority(UART4_RX_DMA_IRQn, 3, 1);
				HAL_NVIC_EnableIRQ(UART4_RX_DMA_IRQn);
#endif
}

void bsp_UART4_init(void)
{
   bsp_uart4_gpio_init();
   bsp_uart4_DMA_cfg();
   bsp_uart4_config();

	  __HAL_UART_CLEAR_FLAG(&huart4,UART_FLAG_TC);

   __HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);

   HAL_UART_Receive_DMA(&huart4,uart4_rx,UART4_RX_MAX);

   UART4_MuxSem_Handle=xSemaphoreCreateMutex();
}

void UART4_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart4);
  if(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_IDLE)!=RESET)
		{
				__HAL_UART_CLEAR_IDLEFLAG(&huart4);
				ESP32_Receive_Set();
		}
}
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void UART4_RX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
}
/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void UART4_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_uart4_tx);
}

USART_cnt_t UART4_cnt={0,0};

void UART4_Cnt_Get(void)
{
   UART4_cnt.cur_cnt=UART4_RX_MAX - (uint16_t)(UART4_RX_DMA_STREAM->NDTR);
}

uint8_t Is_UART4_Received(void)
{
   uint16_t len=UART4_RX_MAX - (uint16_t)(UART4_RX_DMA_STREAM->NDTR);
   return (len != UART4_cnt.old_cnt);
}

void UART4_Cnt_Flush(void)
{
   UART4_cnt.old_cnt=UART4_cnt.cur_cnt;
}

uint16_t UART4_Data_EnQueue(Que_t *que)
{
   if(UART4_cnt.cur_cnt == UART4_cnt.old_cnt)
   {
       return 0;
   }

   if(UART4_cnt.cur_cnt > UART4_cnt.old_cnt)
   {
      EnQueueEx(que,uart4_rx+UART4_cnt.old_cnt,UART4_cnt.cur_cnt - UART4_cnt.old_cnt);
   }
   else
   {
       EnQueueEx(que,uart4_rx+UART4_cnt.old_cnt,UART4_RX_MAX-UART4_cnt.old_cnt);
       EnQueueEx(que,uart4_rx,UART4_cnt.cur_cnt);
   }

   return que->len;
}

uint16_t UART4_Data_Copy(uint8_t *pbuf)
{
   if(UART4_cnt.cur_cnt == UART4_cnt.old_cnt)
   {
       return 0;
   }

   if(UART4_cnt.cur_cnt > UART4_cnt.old_cnt)
   {
      memcpy(pbuf,uart4_rx+UART4_cnt.old_cnt,UART4_cnt.cur_cnt - UART4_cnt.old_cnt);
      return UART4_cnt.cur_cnt - UART4_cnt.old_cnt;
   }

   memcpy(pbuf,uart4_rx+UART4_cnt.old_cnt,UART4_RX_MAX-UART4_cnt.old_cnt);
   memcpy(pbuf,uart4_rx,UART4_cnt.cur_cnt);
   
   return (UART4_RX_MAX-UART4_cnt.old_cnt+UART4_cnt.cur_cnt);
}

void UART4_Data_Printf(void)
{
   if(UART4_cnt.cur_cnt == UART4_cnt.old_cnt)
   {
       return ;
   }

   if(UART4_cnt.cur_cnt > UART4_cnt.old_cnt)
   {
      USART6_data_send(uart4_rx+UART4_cnt.old_cnt,UART4_cnt.cur_cnt - UART4_cnt.old_cnt);
   }
   else
   {
       USART6_data_send(uart4_rx+UART4_cnt.old_cnt,UART4_RX_MAX - UART4_cnt.old_cnt);
       USART6_data_send(uart4_rx,UART4_cnt.cur_cnt);
   }
}
static void UART4_Out_mutex_lock(void)
{

    if ( __get_IPSR() ||(!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }

    xSemaphoreTake(UART4_MuxSem_Handle,portMAX_DELAY);
}

static void UART4_Out_mutex_unlock(void)
{
    if ( __get_IPSR() || (!FreeRTOSIsSchedulerRunning()))
    {
        return;
    }
   xSemaphoreGive(UART4_MuxSem_Handle);
}
/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void UART4_data_send(uint8_t *pData, uint16_t Size)
{
			UART4_Out_mutex_lock();
			HAL_UART_Transmit(&huart4, pData, Size, 1000);
			UART4_Out_mutex_unlock();
}

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void UART4_data_DMA_send(uint8_t *pData, uint16_t Size)
{
			UART4_Out_mutex_lock();
			HAL_UART_Transmit_DMA(&huart4, pData, Size);
			UART4_Out_mutex_unlock();
}


void ESP32_Send_Cmd(void *pstr)
{
			uint16_t i=0;
			uint8_t *ptemp=(uint8_t *)pstr;
			UART4_Out_mutex_lock();
			do 
			{
     HAL_UART_Transmit(&huart4, (ptemp + i), 1, 1000);
					i++;
			} while(*(ptemp + i)!='\0');
			UART4_Out_mutex_unlock();
}


