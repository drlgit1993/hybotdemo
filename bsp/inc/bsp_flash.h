/**
  * @file    bsp_flash.c
  * @author  DRL
  * @version V1.0.0
  * @date    31-July-2020
  * @brief   
  *          	
**/
#ifndef BSP_FLASH_H__
#define BSP_FLASH_H__

#include "stm32f4xx.h"
#include "stdio.h"
#include "stdlib.h"

#define  BACKUP_SIZE             ((uint32_t)0x100)        /*备份长度*/

#define  START_ADDR_SECTOR       ((uint32_t)0x08000000) 	/*FLASH的扇区起始地址*/

#define FLASH_WAITETIME  50000          /*FLASH等待超时时间*/


/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)

#define  END_ADDR_SECTOR         ((uint32_t)0x080FFFFF) 	/*FLASH的扇区终止地址*/

#endif   /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */


/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)

#define  END_ADDR_SECTOR         ((uint32_t)0x0803FFFF) 	/*FLASH的扇区终止地址*/

#endif /* STM32F401xC */


/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
/*FLASH 扇区的起始地址(STM32F40x and STM32F41x)*/
#define  START_ADDR_SECTOR0     ((uint32_t)0x08000000) 	/*扇区0起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR1     ((uint32_t)0x08004000) 	/*扇区1起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR2     ((uint32_t)0x08008000) 	/*扇区2起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR3     ((uint32_t)0x0800C000) 	/*扇区3起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR4     ((uint32_t)0x08010000) 	/*扇区4起始地址, 64 Kbytes*/
#define  START_ADDR_SECTOR5     ((uint32_t)0x08020000) 	/*扇区5起始地址, 128 Kbytes*/
#endif /* STM32F401xC */


/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)

/*FLASH 扇区的起始地址(STM32F40x and STM32F41x)*/
#define  START_ADDR_SECTOR0     ((uint32_t)0x08000000) 	/*扇区0起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR1     ((uint32_t)0x08004000) 	/*扇区1起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR2     ((uint32_t)0x08008000) 	/*扇区2起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR3     ((uint32_t)0x0800C000) 	/*扇区3起始地址, 16 Kbytes*/
#define  START_ADDR_SECTOR4     ((uint32_t)0x08010000) 	/*扇区4起始地址, 64 Kbytes*/
#define  START_ADDR_SECTOR5     ((uint32_t)0x08020000) 	/*扇区5起始地址, 128 Kbytes*/
#define  START_ADDR_SECTOR6     ((uint32_t)0x08040000) 	/*扇区6起始地址, 128 Kbytes*/
#define  START_ADDR_SECTOR7     ((uint32_t)0x08060000) 	/*扇区7起始地址, 128 Kbytes*/
#define  START_ADDR_SECTOR8     ((uint32_t)0x08080000) 	/*扇区8起始地址, 128 Kbytes*/
#define  START_ADDR_SECTOR9     ((uint32_t)0x080A0000) 	/*扇区9起始地址, 128 Kbytes*/
#define  START_ADDR_SECTOR10    ((uint32_t)0x080C0000) 	/*扇区10起始地址,128 Kbytes*/
#define  START_ADDR_SECTOR11    ((uint32_t)0x080E0000) 	/*扇区11起始地址,128 Kbytes*/

#endif   /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */





typedef enum
{
  write_ok=0,
  write_fail,
  addr_err,
  erase_fail,
  erase_ok
}flash_status;

/**
  * @brief  从addr处读取一个字(4字节)
  * @param  addr：读取的地址
  * @retval 读取到的数据
**/
uint32_t read_oneword_flash(uint32_t addr);

/**
  * @brief    :从addr处开始读len个字的数据
  * @param    :addr:读取的首地址，pbuff：读取到的数据缓存，len:需要读取的数据长度
  * @retval   :none
**/
void read_mulWord_flash(uint32_t addr,uint32_t *pbuff,uint32_t len);

/**
  * @brief    :从addr处开始读出len个半字的数据 
  * @param    :addr:待读取的首地址，pbuff：数据缓存，len:需要读取的数据长度
  * @retval   :none
**/
void read_HalfWord_flash(uint32_t addr, uint16_t *pbuff, uint16_t len);

/**
  * @brief    :read len byte date from addr
  * @param    :addr:读取的首地址，pbuff：读取到的数据缓存，len:需要读取的数据长度
  * @retval   :none
**/
void read_mulbyte_flash(uint32_t addr,uint8_t *pbuff,uint32_t len);

/**
  * @brief    :直接写入数据到flash,写入区域已经被擦除.
  * @param    :waddr:起始地址, pbuff:数据指针,wlen：写入的数据长度（单位为字）
  * @retval   :write_ok(写入成功),write_fail(写入失败),addr_err(写入的地址非法)
**/
flash_status noerase_write_flash(uint32_t waddr,uint32_t *pbuff,uint16_t wlen);

/**
  * @brief    :写入区域先擦除，再写入数据。
  * @param    :waddr:写入区域首地址, pbuf:数据缓存,wlen:写入的数据长度（单位为字）
  * @retval   :write_ok(写入数据成功)，addr_err(写入的地址非法），erase_fail（擦除数据失败），write_fail（数据写入失败)
  * @attention:
**/
flash_status nobackup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen);

/**
  * @brief   :先备份BACKUP_SIZE字大小的数据，将待写入的数据插入备份数据中，然后擦除整个扇区，最后将所有备份的数据写入扇区。
  * @param   :waddr:起始地址,pbuf:待写入的数据,wlen：写入的数据长度(单位为字)
  * @retval  :write_ok(写入数据成功)，addr_err(写入的地址非法），erase_fail（擦除数据失败），write_fail（数据写入失败)
**/
flash_status backup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen);

/**
  * @brief   :从addr处开始擦除elen个字的flash
  * @param   :waddr:起始地址,elen：擦除长度(单位为字)
  * @retval  :erase_fail（擦除失败），erase_ok(擦除成功)
**/
flash_status sector_erase_flash(uint32_t addr,uint32_t elen);

#endif       /*BSP_FLASH_H__*/

