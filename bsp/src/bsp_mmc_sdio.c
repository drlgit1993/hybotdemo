/**
  ******************************************************************************
  * File Name          : SDIO.c
  * Description        : This file provides code for the configuration
  *                      of the SDIO instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_mmc_sdio.h"
volatile uint8_t print_time=0;
/* USER CODE BEGIN 0 */
#ifdef USE_MMC_CARD_FATFS
volatile MMC_FlagDef disk_Flag={0};
#endif
/* USER CODE END 0 */
__align(4) uint8_t SDIO_DATA_BUFFER[512];

MMC_HandleTypeDef hmmc;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;


/* SDIO init function */

uint8_t bsp_MMC_Init(void)
{
 uint8_t status=MMC_OK;

  hmmc.Instance = SDIO;
  hmmc.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hmmc.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hmmc.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hmmc.Init.BusWide = SDIO_BUS_WIDE_1B;
  hmmc.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hmmc.Init.ClockDiv = 2;//SDIO_TRANSFER_CLK_DIV;

  HAL_MMC_DeInit(&hmmc);

  if (HAL_MMC_Init(&hmmc) != HAL_OK)
  {
    Error_Handler();
    status = MMC_ERROR;
  }

  if(status==MMC_OK)
  {
    if (HAL_MMC_ConfigWideBusOperation(&hmmc, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
      Error_Handler();
      status = MMC_ERROR;
    }
  } 
 // bsp_MMC_info_get();/*读取MMC信息*/
  return status;
}

void HAL_MMC_MspInit(MMC_HandleTypeDef* mmcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(mmcHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    PA15    ------>  RST
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(SDIO_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    
    __HAL_LINKDMA(mmcHandle,hdmarx,hdma_sdio_rx);
//    HAL_DMA_DeInit(&hdma_sdio_rx);

    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;

    __HAL_LINKDMA(mmcHandle,hdmatx,hdma_sdio_tx);
//    HAL_DMA_DeInit(&hdma_sdio_tx);

    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

  /* USER CODE BEGIN SDIO_MspInit 1 */
  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_MMC_MspDeInit(MMC_HandleTypeDef* mmcHandle)
{

  if(mmcHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(mmcHandle->hdmarx);
    HAL_DMA_DeInit(mmcHandle->hdmatx);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE END SDIO_MspDeInit 1 */
  }
}


uint8_t bsp_MMC_ReadDisk(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
  uint8_t state=MMC_TRANSFER_OK;
	 uint32_t timeout=MMC_TIMEOUT;
 // __disable_irq(); /*关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)*/
  state=HAL_MMC_ReadBlocks(&hmmc,pData,ReadAddr,NumOfBlocks,1000);
	
	 while(HAL_MMC_GetCardState(&hmmc)!=HAL_MMC_CARD_TRANSFER)
  {
		  if(timeout-- == 0)
		  {	
			   return MMC_TRANSFER_BUSY;
		  }
  } 
  //__enable_irq();/*开启总中断*/

  return state;
}  
  
/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */	
uint8_t bsp_MMC_WriteDisk(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{   
  uint8_t sta=MMC_TRANSFER_OK;
	 uint32_t timeout=MMC_TIMEOUT;
  __disable_irq(); /*关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)*/
	 sta=HAL_MMC_WriteBlocks(&hmmc,pData,WriteAddr,NumOfBlocks,1000);
		
	 while(HAL_MMC_GetCardState(&hmmc)!=HAL_MMC_CARD_TRANSFER)
  {
		  if(timeout-- == 0)
		  {	
			   return MMC_TRANSFER_BUSY;
		  }
  }   
	 __enable_irq();/*开启总中断*/
  return sta;
}
/* USER CODE BEGIN 1 */

/**
  * @brief  Reads block(s) from a specified address in MMC card,in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval none
*/
uint8_t bsp_MMC_ReadBlocks(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
if(HAL_MMC_ReadBlocks(&hmmc,pData,BlockAdd,NumberOfBlocks,1000)==HAL_OK)
 {
    BSP_MMC_SDIO_LOG("MMC DMA blocks read successfuly in polling mode!");
    return MMC_OK;
 }
 BSP_MMC_SDIO_LOG("MMC DMA blocks read Fail in polling mode!");
 return MMC_ERROR;
}
/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval none
*/
uint8_t bsp_MMC_ReadBlocks_DMA(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
 if(HAL_MMC_ReadBlocks_DMA(&hmmc,pData,BlockAdd,NumberOfBlocks)==HAL_OK)
 {
    BSP_MMC_SDIO_LOG("MMC DMA blocks read successfuly in DMA mode!");
    return MMC_OK;
 }
 BSP_MMC_SDIO_LOG("MMC DMA blocks read Fail in DMA mode!");
 return MMC_ERROR;
}

/**
  * @brief  Writes block(s) to a specified address in MMC card,in polling mode.  
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval none 
  */
uint8_t bsp_MMC_WriteBlocks(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
if(HAL_MMC_WriteBlocks(&hmmc,pData,BlockAdd,NumberOfBlocks,1000)==HAL_OK)
 {
   BSP_MMC_SDIO_LOG("MMC DMA blocks write successfuly in polling mode!");
   return MMC_OK;
 }
 BSP_MMC_SDIO_LOG("MMC DMA blocks write fail in polling mode!");
 return MMC_ERROR;
}

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval none 
  */
uint8_t bsp_MMC_WriteBlocks_DMA(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks)
{
 if(HAL_MMC_WriteBlocks_DMA(&hmmc,pData,BlockAdd,NumberOfBlocks) == HAL_OK)
 {
   BSP_MMC_SDIO_LOG("MMC DMA blocks write successfuly in DMA mode!");
   return MMC_OK;
 }
 BSP_MMC_SDIO_LOG("MMC DMA blocks write fail in DMA mode!");
 return MMC_ERROR;
}
/**
  * @brief  Erases the specified memory area of the given MMC card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval MMC status
  */
uint8_t bsp_MMC_erase(uint32_t StartAddr, uint32_t EndAddr)
{
  if(HAL_MMC_Erase(&hmmc,StartAddr,EndAddr)!=HAL_OK)
  {
    BSP_MMC_SDIO_LOG("MMC erase  Fail!");
    return MMC_ERROR;
  }
  return MMC_OK;
}
/**
  * @brief  Gets the current MMC card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  MMC_TRANSFER_OK: No data transfer is acting
  *            @arg  MMC_TRANSFER_BUSY: Data transfer is acting
  */
uint8_t bsp_MMC_GetCardStatus(void)
{
   return ((HAL_MMC_GetCardState(&hmmc)== HAL_MMC_CARD_TRANSFER) ? MMC_TRANSFER_OK:MMC_TRANSFER_BUSY);
}

/**
  * @brief  Get MMC information about specific MMC card.
  * @param  pInfo: Pointer to HAL_MMC_CardInfoTypeDef structure
  * @retval None 
  */
void bsp_MMC_GetCardInfo(HAL_MMC_CardInfoTypeDef *pInfo)
{
  HAL_MMC_GetCardInfo(&hmmc,pInfo);
  MMC_Read_ExtCsd(&hmmc);
  pInfo->LogBlockNbr=hmmc.MmcCard.CardCap;
}

#ifdef USE_MMC_CARD_FATFS
/**
  * @brief This function handles SDIO global interrupt.
  */
void SDIO_IRQHandler(void)
{
  /* USER CODE BEGIN SDIO_IRQn 0 */

  /* USER CODE END SDIO_IRQn 0 */
  HAL_MMC_IRQHandler(&hmmc);
  /* USER CODE BEGIN SDIO_IRQn 1 */

  /* USER CODE END SDIO_IRQn 1 */
}
/**
  * @brief Tx Transfer completed callbacks
  * @param hmmc: Pointer to MMC handle
  * @retval None
  */
void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hmmc)
{
  disk_Flag.Tx=1;
}
void HAL_MMC_ErrorCallback(MMC_HandleTypeDef *hmmc)
{
  printf("MMC error\r\n");
}
/**
  * @brief Rx Transfer completed callbacks
  * @param hmmc: Pointer MMC handle
  * @retval None
  */
void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hmmc)
{
  disk_Flag.Rx=1;
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_rx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_tx);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}
#endif
/*
void MMC_Capacity_get(void)
{
uint16_t index=0,cnt=0;
uint32_t Capacity_hex=0;
  uint32_t  Capacity =0;
  HAL_StatusTypeDef state=HAL_OK;
  state = MMC_Read_ExtCsd(&hmmc);
  printf("SEC_COUNT[%d]:%#x\r\n",0,(uint32_t)hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[0]);
  printf("SEC_COUNT[%d]:%#x\r\n",1,((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[1])<<8));
  printf("SEC_COUNT[%d]:%#x\r\n",2,((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[2])<<16));
  printf("SEC_COUNT[%d]:%#x\r\n",3,((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[3])<<24));

  Capacity_hex=(((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[3])<<24) |\
           ((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[2])<<16) |\
           ((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[1])<<8) |\
           ((uint32_t)(hmmc.MmcCard.ExtCsd.EXT_CSD.SEC_COUNT[0]))) >> 11;

  Capacity=Capacity_hex >> 11;

  printf("MMC Capacity_hex:%#x\r\n",Capacity_hex);
  printf("MMC Capacity:%d M byte\r\n",Capacity);
 printf("MMC Capacity:%d M byte\r\n",hmmc.MmcCard.CardCap);
for(index=0;index<MMC_BLOCKSIZE;index++)
{
  cnt ++;
  printf("%d:%#x\t",index,hmmc.MmcCard.ExtCsd.CSDBuf[index]);
 if((cnt%10)==0)
{
 printf("\r\n");
}
}
}
*/
/*
static void bsp_MMC_info_get(void)
{
 HAL_MMC_CardCIDTypeDef CardCID={0};
 HAL_MMC_CardInfoTypeDef pCardInfo={0};
 uint8_t CBX=0;
 uint8_t OID=0;
 HAL_MMC_GetCardCID(&hmmc,&CardCID);
 CBX=(CardCID.OEM_AppliID &0xff00)>>8;
 OID=CardCID.OEM_AppliID &0x00ff;
 printf("MID:%#x,CBX:%#x,OID:%#x,PNM:%#x%x%x\r\n",CardCID.ManufacturerID,CBX,OID,CardCID.ProdName1,CardCID.ProdName2,CardCID.ProdRev);

 bsp_MMC_GetCardInfo(&pCardInfo);
 printf("CardType:%#x,Class:%#x,RelCardAdd:%#x,BlockNbr:%#x,BlockSize:%#x,LogBlockNbr:%#x,LogBlockSize:%#x\r\n", \
           pCardInfo.CardType,pCardInfo.Class,pCardInfo.RelCardAdd,pCardInfo.BlockNbr,pCardInfo.BlockSize,pCardInfo.LogBlockNbr,pCardInfo.LogBlockSize);
//MMC_Capacity_get();
}*/
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
