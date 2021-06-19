/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2019          /
/-----------------------------------------------------------------------*/


/*
  ==============================================================================
                       ##### MMC卡与SD卡使用说明 #####
  ============================================================================== 
<1>、使用SD卡时,应定义宏USE_SD_CARD_FATFS;
<2>、使用SD卡时,需要添加以下文件：
					(1)、bsp_sd_sdio.c,
					(2)、bsp_disk_fatfs.c,
					(3)ffsystem.c,
					(4)ffunicode.c,
					(5)、ff.c,
					(6)、diskio.c;
<3>、使用SD卡时,需要添加的头文件有bsp_sd_sdio.h,bsp_disk_fatfs.h,user_diskio.h以及fatfs文件夹下所有头文件;

<4>、使用MMC卡时,应定义宏USE_MMC_CARD_FATFS;
<5>、使用SD卡时,需要添加以下文件：
					(1)、bsp_MMC_sdio.c,
					(2)、bsp_disk_fatfs.c,
					(3)ffsystem.c,
					(4)ffunicode.c,
					(5)、ff.c,
					(6)、diskio.c;
<6>、使用SD卡时,需要添加的头文件有bsp_MMC_sdio.h,bsp_disk_fatfs.h,user_diskio.h以及fatfs文件夹下所有头文件;
<7>、宏USE_SD_CARD_FATFS与USE_MMC_CARD_FATFS不能同时定义使用!!!!!!!!!!
*/


#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef USE_SD_CARD_FATFS
#include "bsp_sd_sdio.h"
#endif

#ifdef USE_MMC_CARD_FATFS
#include "bsp_mmc_sdio.h"
#endif

#include <string.h>
#include <stdio.h>
#include "ff.h"

#if defined(USE_SD_CARD_FATFS) &&defined(USE_MMC_CARD_FATFS)
 #error please defined USE_SD_CARD_FATFS or USE_MMC_CARD_FATFS,not and !
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */




#ifdef USE_SD_CARD_FATFS
  #define DISK_OK                                                    HAL_OK
  #define disk_Flag_Def                                              SD_FlagDef
  #define DISK_TIMEOUT                                               SD_TIMEOUT
  #define DISK_BLOCK_SIZE                                            SD_BLOCK_SIZE
  #define DISK_TRANSFER_OK                                           SD_TRANSFER_OK
  #define disk_CardInfoTypeDef                                       HAL_SD_CardInfoTypeDef
  #define disk_init()                                                bsp_SD_Init()
  #define disk_GetCardState()                                        bsp_SD_GetCardState()
  #define disk_ReadBlocks_DMA(pData,BlockAdd,NumberOfBlocks)         bsp_SD_ReadBlocks_DMA(pData,BlockAdd,NumberOfBlocks)
  #define disk_WriteBlocks_DMA(pData,BlockAdd,NumberOfBlocks)        bsp_SD_WriteBlocks_DMA(pData,BlockAdd,NumberOfBlocks)
  #define disk_GetCardInfo(pInfo)                                    bsp_SD_GetCardInfo(pInfo)
#endif


 DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif 

#ifdef __cplusplus
}
#endif

#endif
