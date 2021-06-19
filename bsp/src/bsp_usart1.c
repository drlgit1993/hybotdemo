#include "bsp_usart1.h"


static UART_HandleTypeDef huart1;
static DMA_HandleTypeDef  hdma_usart1_tx;
static DMA_HandleTypeDef  hdma_usart1_rx;

static uint8_t usart1_rx[USART1_RX_MAX]={0};


static void bsp_usart1_config(void)
{
  USART1_CLK_ENABLE();

  huart1.Instance = USART1;
  huart1.Init.BaudRate = USART1_BAUDRATE;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

#if defined(USE_USART1_RX) && !defined(USE_USART1_TX)
  huart1.Init.Mode = UART_MODE_RX;
#endif

#if defined(USE_USART1_TX) && !defined(USE_USART1_RX)
  huart1.Init.Mode = UART_MODE_TX;
#endif

#if defined(USE_USART1_RX) && defined(USE_USART1_TX)
  huart1.Init.Mode = UART_MODE_TX_RX;
#endif

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(USART1_IRQn, USART1_MAINPRI,USART1_SUBPRI);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void bsp_usart1_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
 
#if defined(USE_USART1_RX)|| defined(USE_USART1_TX)

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART1_AF;
#endif

#ifdef USE_USART1_RX
    USART1_RX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART1_RX_PIN;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);
#endif

#ifdef USE_USART1_TX
    USART1_TX_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = USART1_TX_PIN;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

void bsp_usart1_gpio_Deinit(void)
{
    __HAL_RCC_USART1_CLK_DISABLE();

#ifdef USE_USART1_RX
    HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_PIN);
    HAL_DMA_DeInit(huart1.hdmarx);
#endif

#ifdef USE_USART1_TX
 HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_PIN);
 HAL_DMA_DeInit(huart1.hdmatx);
#endif
 /* USART1 interrupt Deinit */
 HAL_NVIC_DisableIRQ(USART1_IRQn);
}

static void bsp_usart1_DMA_cfg(void)
{
#ifdef USE_USART1_TX
    /* DMA controller clock enable */
				__HAL_RCC_DMA2_CLK_ENABLE();
    /* USART1_RX Init */
    hdma_usart1_tx.Instance = USART1_TX_DMA_STREAM;
    hdma_usart1_tx.Init.Channel = USART1_TX_DMA_CHANNEL;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_usart1_tx.Init.MemBurst=DMA_MBURST_INC4;
    hdma_usart1_tx.Init.PeriphBurst=DMA_PBURST_INC4;

    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);

				HAL_NVIC_SetPriority(USART1_TX_DMA_IRQn, 3, 2);
				HAL_NVIC_EnableIRQ(USART1_TX_DMA_IRQn);
#endif

#ifdef USE_USART1_RX
    /* DMA controller clock enable */
				__HAL_RCC_DMA2_CLK_ENABLE();
    hdma_usart1_rx.Instance = USART1_RX_DMA_STREAM;
    hdma_usart1_rx.Init.Channel = USART1_RX_DMA_CHANNEL;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_usart1_rx.Init.MemBurst=DMA_MBURST_INC4;
    hdma_usart1_rx.Init.PeriphBurst=DMA_PBURST_INC4;

    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);


				HAL_NVIC_SetPriority(USART1_RX_DMA_IRQn, 2, 1);
				HAL_NVIC_EnableIRQ(USART1_RX_DMA_IRQn);
#endif
}

void bsp_USART1_init(void)
{
  bsp_usart1_gpio_init();
  
  bsp_usart1_DMA_cfg();
 
  bsp_usart1_config();  

  HAL_UART_Receive_DMA(&huart1,usart1_rx,USART1_RX_MAX); 
  
	 __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_TC);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void USART1_RX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}
/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void USART1_TX_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

USART_cnt_t USART1_cnt={0,0};

void USART1_Cnt_Get(void)
{
   USART1_cnt.cur_cnt=USART1_RX_MAX - (uint16_t)(USART1_RX_DMA_STREAM->NDTR);
}

uint8_t Is_USART1_Received(void)
{
   uint16_t len=USART1_RX_MAX - (uint16_t)(USART1_RX_DMA_STREAM->NDTR);
   return (len != USART1_cnt.old_cnt);
}

void USART1_Cnt_Flush(void)
{
   USART1_cnt.old_cnt=USART1_cnt.cur_cnt;
}

uint16_t USART1_Data_EnQueue(Que_t *que)
{
   if(USART1_cnt.cur_cnt == USART1_cnt.old_cnt)
   {
       return 0;
   }

   if(USART1_cnt.cur_cnt > USART1_cnt.old_cnt)
   {
      EnQueueEx(que,usart1_rx+USART1_cnt.old_cnt,USART1_cnt.cur_cnt - USART1_cnt.old_cnt);
   }
   else
   {
       EnQueueEx(que,usart1_rx+USART1_cnt.old_cnt,USART1_RX_MAX-USART1_cnt.old_cnt);
       EnQueueEx(que,usart1_rx,USART1_cnt.cur_cnt);
   }

   return que->len;
}
void USART1_Data_Printf(void)
{
   if(USART1_cnt.cur_cnt == USART1_cnt.old_cnt)
   {
       return ;
   }

   if(USART1_cnt.cur_cnt > USART1_cnt.old_cnt)
   {
      USART6_data_send(usart1_rx+USART1_cnt.old_cnt,USART1_cnt.cur_cnt - USART1_cnt.old_cnt);
   }
   else
   {
       USART6_data_send(usart1_rx+USART1_cnt.old_cnt,USART1_RX_MAX - USART1_cnt.old_cnt);
       USART6_data_send(usart1_rx,USART1_cnt.cur_cnt);
   }
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART1_data_send(uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit(&huart1, pData, Size, 1000);
}

/**
  * @brief  Sends an amount of data in DMA mode.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @retval none 
  */
void USART1_data_DMA_send(uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit_DMA(&huart1, pData, Size);

  while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY)
  {
  } 

}


