/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "FreeRTOS.h"
#include "task.h" 

static volatile DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	
//  DSTATUS state=STA_NOINIT;

//  if(disk_GetCardState() == DISK_TRANSFER_OK)
//  {
//     state &= ~STA_NOINIT;
//  }
//  return state;
 return Stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	//  if(Stat != STA_NOINIT)
//  {
//      return 0;
//  }

  if(disk_init() == DISK_OK)
  {    
      Stat &= ~STA_NOINIT;
  }
  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	  DRESULT res = RES_ERROR;
   res=(DRESULT)bsp_SD_ReadDisk((uint8_t*)buff,(uint32_t)sector,count);
   return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
   DRESULT res = RES_ERROR;
   res= (DRESULT)bsp_SD_WriteDisk((uint8_t*)buff,(uint32_t)sector,count);
   return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_ERROR;
    disk_CardInfoTypeDef CardInfo={0};

    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (cmd)
    {
      /* Make sure that no pending write process */
      case CTRL_SYNC :
      { 
        res = RES_OK;
      }break;

      /* Get number of sectors on the disk (DWORD) */
      case GET_SECTOR_COUNT :
      {
        disk_GetCardInfo(&CardInfo);
        *(DWORD*)buff = CardInfo.LogBlockNbr;
        res = RES_OK;
      }break;

      /* Get R/W sector size (WORD) */
      case GET_SECTOR_SIZE :
      {
        disk_GetCardInfo(&CardInfo);
        *(WORD*)buff = CardInfo.LogBlockSize;
        res = RES_OK;
      }break;

      /* Get erase block size in unit of sector (DWORD) */
      case GET_BLOCK_SIZE :
      {
        disk_GetCardInfo(&CardInfo);
        *(DWORD*)buff = CardInfo.LogBlockNbr / DISK_BLOCK_SIZE;
        res = RES_OK;
      }break;

      default:res = RES_PARERR;
    }
    return res;   
}

