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

#define  BACKUP_SIZE             ((uint32_t)0x100)        /*���ݳ���*/

#define  START_ADDR_SECTOR       ((uint32_t)0x08000000) 	/*FLASH��������ʼ��ַ*/

#define FLASH_WAITETIME  50000          /*FLASH�ȴ���ʱʱ��*/


/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)

#define  END_ADDR_SECTOR         ((uint32_t)0x080FFFFF) 	/*FLASH��������ֹ��ַ*/

#endif   /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */


/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)

#define  END_ADDR_SECTOR         ((uint32_t)0x0803FFFF) 	/*FLASH��������ֹ��ַ*/

#endif /* STM32F401xC */


/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
/*FLASH ��������ʼ��ַ(STM32F40x and STM32F41x)*/
#define  START_ADDR_SECTOR0     ((uint32_t)0x08000000) 	/*����0��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR1     ((uint32_t)0x08004000) 	/*����1��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR2     ((uint32_t)0x08008000) 	/*����2��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR3     ((uint32_t)0x0800C000) 	/*����3��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR4     ((uint32_t)0x08010000) 	/*����4��ʼ��ַ, 64 Kbytes*/
#define  START_ADDR_SECTOR5     ((uint32_t)0x08020000) 	/*����5��ʼ��ַ, 128 Kbytes*/
#endif /* STM32F401xC */


/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)

/*FLASH ��������ʼ��ַ(STM32F40x and STM32F41x)*/
#define  START_ADDR_SECTOR0     ((uint32_t)0x08000000) 	/*����0��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR1     ((uint32_t)0x08004000) 	/*����1��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR2     ((uint32_t)0x08008000) 	/*����2��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR3     ((uint32_t)0x0800C000) 	/*����3��ʼ��ַ, 16 Kbytes*/
#define  START_ADDR_SECTOR4     ((uint32_t)0x08010000) 	/*����4��ʼ��ַ, 64 Kbytes*/
#define  START_ADDR_SECTOR5     ((uint32_t)0x08020000) 	/*����5��ʼ��ַ, 128 Kbytes*/
#define  START_ADDR_SECTOR6     ((uint32_t)0x08040000) 	/*����6��ʼ��ַ, 128 Kbytes*/
#define  START_ADDR_SECTOR7     ((uint32_t)0x08060000) 	/*����7��ʼ��ַ, 128 Kbytes*/
#define  START_ADDR_SECTOR8     ((uint32_t)0x08080000) 	/*����8��ʼ��ַ, 128 Kbytes*/
#define  START_ADDR_SECTOR9     ((uint32_t)0x080A0000) 	/*����9��ʼ��ַ, 128 Kbytes*/
#define  START_ADDR_SECTOR10    ((uint32_t)0x080C0000) 	/*����10��ʼ��ַ,128 Kbytes*/
#define  START_ADDR_SECTOR11    ((uint32_t)0x080E0000) 	/*����11��ʼ��ַ,128 Kbytes*/

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
  * @brief  ��addr����ȡһ����(4�ֽ�)
  * @param  addr����ȡ�ĵ�ַ
  * @retval ��ȡ��������
**/
uint32_t read_oneword_flash(uint32_t addr);

/**
  * @brief    :��addr����ʼ��len���ֵ�����
  * @param    :addr:��ȡ���׵�ַ��pbuff����ȡ�������ݻ��棬len:��Ҫ��ȡ�����ݳ���
  * @retval   :none
**/
void read_mulWord_flash(uint32_t addr,uint32_t *pbuff,uint32_t len);

/**
  * @brief    :��addr����ʼ����len�����ֵ����� 
  * @param    :addr:����ȡ���׵�ַ��pbuff�����ݻ��棬len:��Ҫ��ȡ�����ݳ���
  * @retval   :none
**/
void read_HalfWord_flash(uint32_t addr, uint16_t *pbuff, uint16_t len);

/**
  * @brief    :read len byte date from addr
  * @param    :addr:��ȡ���׵�ַ��pbuff����ȡ�������ݻ��棬len:��Ҫ��ȡ�����ݳ���
  * @retval   :none
**/
void read_mulbyte_flash(uint32_t addr,uint8_t *pbuff,uint32_t len);

/**
  * @brief    :ֱ��д�����ݵ�flash,д�������Ѿ�������.
  * @param    :waddr:��ʼ��ַ, pbuff:����ָ��,wlen��д������ݳ��ȣ���λΪ�֣�
  * @retval   :write_ok(д��ɹ�),write_fail(д��ʧ��),addr_err(д��ĵ�ַ�Ƿ�)
**/
flash_status noerase_write_flash(uint32_t waddr,uint32_t *pbuff,uint16_t wlen);

/**
  * @brief    :д�������Ȳ�������д�����ݡ�
  * @param    :waddr:д�������׵�ַ, pbuf:���ݻ���,wlen:д������ݳ��ȣ���λΪ�֣�
  * @retval   :write_ok(д�����ݳɹ�)��addr_err(д��ĵ�ַ�Ƿ�����erase_fail����������ʧ�ܣ���write_fail������д��ʧ��)
  * @attention:
**/
flash_status nobackup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen);

/**
  * @brief   :�ȱ���BACKUP_SIZE�ִ�С�����ݣ�����д������ݲ��뱸�������У�Ȼ���������������������б��ݵ�����д��������
  * @param   :waddr:��ʼ��ַ,pbuf:��д�������,wlen��д������ݳ���(��λΪ��)
  * @retval  :write_ok(д�����ݳɹ�)��addr_err(д��ĵ�ַ�Ƿ�����erase_fail����������ʧ�ܣ���write_fail������д��ʧ��)
**/
flash_status backup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen);

/**
  * @brief   :��addr����ʼ����elen���ֵ�flash
  * @param   :waddr:��ʼ��ַ,elen����������(��λΪ��)
  * @retval  :erase_fail������ʧ�ܣ���erase_ok(�����ɹ�)
**/
flash_status sector_erase_flash(uint32_t addr,uint32_t elen);

#endif       /*BSP_FLASH_H__*/

