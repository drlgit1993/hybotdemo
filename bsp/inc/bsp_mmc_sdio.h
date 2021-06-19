/**
  ******************************************************************************
  * File Name          : SDIO.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BSP_MMC_SDIO_H
#define _BSP_MMC_SDIO_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_dwt.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern MMC_HandleTypeDef hmmc;

typedef struct
{
 uint8_t Tx;
 uint8_t Rx;
}MMC_FlagDef;

#define MMC_SDIO_LOG_EN 0

#if MMC_SDIO_LOG_EN==1

#define  BSP_MMC_SDIO_LOG(...) \
{ \
  printf(__VA_ARGS__); \
  printf("\r\n"); \
}
#else
#define  BSP_MMC_SDIO_LOG(...) 
#endif


/* USER CODE BEGIN Private defines */
#define NUMBER_OF_BLOCK      (1)
#define MMC_TIMEOUT          ((uint32_t)0x00100000U)

/** 
  * @brief  MMC status structure definition  
  */     
#define   MMC_OK         0x00
#define   MMC_ERROR      0x01

/** 
  * @brief  MMC transfer state definition  
  */     
#define   MMC_TRANSFER_OK                ((uint8_t)0x00)
#define   MMC_TRANSFER_BUSY              ((uint8_t)0x01)

/* USER CODE END Private defines */

uint8_t bsp_MMC_Init(void);

/* USER CODE BEGIN Prototypes */

/**
  * @brief  Reads block(s) from a specified address in MMC card,in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval none
*/
uint8_t bsp_MMC_ReadBlocks(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks);

/**
  * @brief  Writes block(s) to a specified address in MMC card,in polling mode.  
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval none 
  */
uint8_t bsp_MMC_WriteBlocks(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks);

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval none
*/
uint8_t bsp_MMC_ReadBlocks_DMA(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks);

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval none 
  */
uint8_t bsp_MMC_WriteBlocks_DMA(uint8_t *pData,uint32_t BlockAdd,uint32_t NumberOfBlocks);

/**
  * @brief  Erases the specified memory area of the given MMC card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval MMC status
  */
uint8_t bsp_MMC_erase(uint32_t StartAddr, uint32_t EndAddr);

/**
  * @brief  Gets the current MMC card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  MMC_TRANSFER_OK: No data transfer is acting
  *            @arg  MMC_TRANSFER_BUSY: Data transfer is acting
  */
uint8_t bsp_MMC_GetCardStatus(void);

/**
  * @brief  Get MMC information about specific MMC card.
  * @param  pInfo: Pointer to HAL_MMC_CardInfoTypeDef structure
  * @retval None 
  */
void bsp_MMC_GetCardInfo(HAL_MMC_CardInfoTypeDef *pInfo);

uint8_t bsp_MMC_ReadDisk(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t bsp_MMC_WriteDisk(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);

/* USER CODE END Prototypes */


#ifdef __cplusplus
}
#endif
#endif /*__ _BSP_MMC_SDIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
