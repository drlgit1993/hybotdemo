/**
  * @file    bsp_flash.c
  * @author  DRL
  * @version V1.0.0
  * @date    31-July-2020
  * @brief   
  *          	
**/
#include "bsp_flash.h"

typedef struct 
{
  uint16_t sector;
  uint32_t satrtaddr;
  uint32_t size;
}flash_sector_t;

flash_sector_t flash_sector;

/**
  * @brief    :����������ַ�������׵�ַ��������С
  * @param    :sector��������ַ��satrtaddr�������׵�ַ��size��������С
  * @retval   :none
**/
static void flash_sector_info_set(uint16_t sector,uint32_t satrtaddr,uint32_t size)
{
  flash_sector.sector=sector;
  flash_sector.satrtaddr=satrtaddr;
  flash_sector.size=size;
}

/**
  * @brief    :��ȡ��ַaddr���ڵ�flash�����������׵�ַ��������С
  * @param    :addr��flash��ַ
  * @retval   :none
**/
/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)

static void get_sector_info(uint32_t addr)
{
  if((addr<START_ADDR_SECTOR0)||(addr>END_ADDR_SECTOR))
  {
    flash_sector_info_set(0xFFFF,0xFFFF,0);
    return ;
  }
  else if(addr < START_ADDR_SECTOR1)
  {
    flash_sector_info_set(FLASH_SECTOR_0,START_ADDR_SECTOR0,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR2)
  {
    flash_sector_info_set(FLASH_SECTOR_1,START_ADDR_SECTOR1,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR3)
  {
    flash_sector_info_set(FLASH_SECTOR_2,START_ADDR_SECTOR2,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR4)
  {
    flash_sector_info_set(FLASH_SECTOR_3,START_ADDR_SECTOR3,0x10000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR5)
  {
    flash_sector_info_set(FLASH_SECTOR_4,START_ADDR_SECTOR4,0x20000);
    return ;
  }
  else
  {
    flash_sector_info_set(FLASH_SECTOR_5,START_ADDR_SECTOR5,0x20000);
    return ;
  }
}
#endif  /* STM32F401xC */

/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) 
static void get_sector_info(uint32_t addr)
{
  
  if((addr<START_ADDR_SECTOR0)||(addr>END_ADDR_SECTOR))
  {
    flash_sector_info_set(0xFFFF,0xFFFF,0);
    return ;
  }
  else if(addr < START_ADDR_SECTOR1)
  {
    flash_sector_info_set(FLASH_SECTOR_0,START_ADDR_SECTOR0,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR2)
  {
    flash_sector_info_set(FLASH_SECTOR_1,START_ADDR_SECTOR1,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR3)
  {
    flash_sector_info_set(FLASH_SECTOR_2,START_ADDR_SECTOR2,0x4000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR4)
  {
    flash_sector_info_set(FLASH_SECTOR_3,START_ADDR_SECTOR3,0x10000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR5)
  {
    flash_sector_info_set(FLASH_SECTOR_4,START_ADDR_SECTOR4,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR6)
  {
    flash_sector_info_set(FLASH_SECTOR_5,START_ADDR_SECTOR5,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR7)
  {
    flash_sector_info_set(FLASH_SECTOR_6,START_ADDR_SECTOR6,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR8)
  {
    flash_sector_info_set(FLASH_SECTOR_7,START_ADDR_SECTOR7,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR9)
  {
    flash_sector_info_set(FLASH_SECTOR_8,START_ADDR_SECTOR8,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR10)
  {
    flash_sector_info_set(FLASH_SECTOR_9,START_ADDR_SECTOR9,0x20000);
    return ;
  }
  else if(addr < START_ADDR_SECTOR11)
  {
    flash_sector_info_set(FLASH_SECTOR_10,START_ADDR_SECTOR10,0x20000);
    return ;
  }
  else
  {
    flash_sector_info_set(FLASH_SECTOR_11,START_ADDR_SECTOR11,0x20000);
    return ;
  }
}
#endif  /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */

/**
  * @brief  ��addr����ȡһ����(4�ֽ�)
  * @param  addr����ȡ�ĵ�ַ
  * @retval ��ȡ��������
**/
uint32_t read_oneword_flash(uint32_t addr)
{
  return *(__IO uint32_t*)addr;
}

/**
  * @brief    :��addr����ʼ��len���ֵ�����
  * @param    :addr:��ȡ���׵�ַ��pbuff����ȡ�������ݻ��棬len:��Ҫ��ȡ�����ݳ���/sizeof(uint32_t)
  * @retval   :none
**/
void read_mulWord_flash(uint32_t addr,uint32_t *pbuff,uint32_t len)
{
  while(len--)
  {
    *pbuff ++ = *(__IO uint32_t *)addr;
    addr +=4;
  }
}
/**
  * @brief    :read len byte date from addr
  * @param    :addr:��ȡ���׵�ַ��pbuff����ȡ�������ݻ��棬len:��Ҫ��ȡ�����ݳ���
  * @retval   :none
**/
void read_mulbyte_flash(uint32_t addr,uint8_t *pbuff,uint32_t len)
{
  while(len--)
  {
    *pbuff ++ = *(__IO uint8_t *)addr;
    addr ++;
  }
}
/**
  * @brief    :��addr����ʼ����len�����ֵ�����
  * @param    :addr:����ȡ���׵�ַ��pbuff�����ݻ��棬len:��Ҫ��ȡ�����ݳ���
  * @retval   :none
**/
void read_HalfWord_flash(uint32_t addr, uint16_t *pbuff, uint16_t len)
{
  while (len--)
  {
    *pbuff = *(__IO uint16_t*)addr;
    pbuff++;
    addr +=2;
  }
}

/**
  * @brief   :��waddrдwlen�ֽڵ�����
  * @param   :waddr:�׵�ַ��pbuff������ָ��,wlen:��д������ݳ���(��λΪ��)
  * @retval  :write_ok(д��ɹ�),write_fail(д��ʧ��)
**/
static flash_status base_write_flash(uint32_t waddr,uint32_t *pbuff,uint16_t wlen)
{
  uint32_t caddr = waddr;
	
  HAL_StatusTypeDef status = HAL_OK;

   while(wlen--)
  {
    status=HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,caddr,*pbuff);
    if(status!=HAL_OK)
    {
      return write_fail;	/*д��ʧ��*/
    }
    caddr += 4;
    pbuff ++;
  }
   return write_ok;
 }

/**
  * @brief    :ֱ��д�����ݵ�flash,д�������Ѿ�������.
  * @param    :waddr:��ʼ��ַ, pbuff:����ָ��,wlen��д������ݳ��ȣ���λΪ�֣�
  * @retval   :write_ok(д��ɹ�),write_fail(д��ʧ��),addr_err(д��ĵ�ַ�Ƿ�)
**/
flash_status noerase_write_flash(uint32_t waddr,uint32_t *pbuff,uint16_t wlen)
{
    flash_status retval=write_fail;

    if((waddr<=START_ADDR_SECTOR)||(waddr>=END_ADDR_SECTOR)||(waddr & 3))
    {
      return addr_err;
    }

    __disable_irq();        
    HAL_FLASH_Unlock();

/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_RDERR);
#endif  

/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
#endif

	retval = base_write_flash(waddr,pbuff,wlen);
    //HAL_Delay(10);

    HAL_FLASH_Lock();
    __enable_irq(); 
    return retval;
}

/**
  * @brief    :д�������Ȳ�������д�����ݡ�
  * @param    :waddr:д�������׵�ַ, pbuf:���ݻ���,wlen:д������ݳ��ȣ���λΪ�֣�
  * @retval   :write_ok(д�����ݳɹ�)��addr_err(д��ĵ�ַ�Ƿ�����erase_fail����������ʧ�ܣ���write_fail������д��ʧ��)
  * @attention:
**/
flash_status nobackup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen)
{
	flash_status retval = write_fail;
	FLASH_EraseInitTypeDef FlashEraseInit;
	uint32_t caddr = waddr;
	uint32_t Wendaddr = waddr + ( wlen << 2 );
	uint32_t SectorError=0;

	if((caddr<=START_ADDR_SECTOR)||(caddr>=END_ADDR_SECTOR)||(caddr & 3))
	{
	return addr_err;
	}

	__disable_irq();        /*��ֹ�ж�*/
	HAL_FLASH_Unlock();

	/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_RDERR);
#endif  

/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
#endif

	FLASH_WaitForLastOperation(FLASH_WAITETIME); 
	/*������д�������*/
	while(caddr<Wendaddr)
	{
   
		if((*(uint32_t*)caddr)!=0XFFFFFFFF)/*��ǰ��ַ�ǿգ�������ǰ��ַ��������*/
		{
			get_sector_info(caddr);

			FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       
			FlashEraseInit.Sector=flash_sector.sector;   
			FlashEraseInit.NbSectors=1;                             
			FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      
			if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
			{
				return erase_fail;/*����ʧ��*/
			}
			else
			{
				if(Wendaddr<(flash_sector.satrtaddr+flash_sector.size))
				{
				  break;
				}
				else
				{
				  caddr = flash_sector.satrtaddr+flash_sector.size;
				} 
			}  
		}
		else
		{
		  caddr += 4;
		}
		FLASH_WaitForLastOperation(FLASH_WAITETIME); 
	}

    retval = base_write_flash(waddr,pbuf,wlen);

    //HAL_Delay(10);
    
    HAL_FLASH_Lock();

    __enable_irq();      /*�����ж�*/

    return retval;
}

/**
  * @brief   :�ȱ���BACKUP_SIZE�ִ�С�����ݣ�����д������ݲ��뱸�������У�Ȼ���������������������б��ݵ�����д��������
  * @param   :waddr:��ʼ��ַ,pbuf:��д�������,wlen��д������ݳ���(��λΪ��)
  * @retval  :write_ok(д�����ݳɹ�)��addr_err(д��ĵ�ַ�Ƿ�����erase_fail����������ʧ�ܣ���write_fail������д��ʧ��)
**/
flash_status backup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen)
{
	flash_status retval = write_fail;
	uint32_t waddrOffset=0;
	uint32_t *backup_buf=NULL;
	uint32_t *tpbuf=NULL;

	get_sector_info(waddr);
	waddrOffset=(waddr-flash_sector.satrtaddr)>>2;/*��ȡ��д���׵�ַ�뱾�����׵�ַ��ƫ����,��λΪ��*/
	backup_buf=(uint32_t*)malloc(sizeof(uint32_t)*BACKUP_SIZE);
	tpbuf=backup_buf+waddrOffset;
	read_mulWord_flash(flash_sector.satrtaddr,backup_buf,BACKUP_SIZE);/*��������*/

	while(wlen--)/*����д������ݲ��뱸������*/
	{
		*tpbuf++ = *pbuf++;
	}

	retval = nobackup_write_flash(flash_sector.satrtaddr,backup_buf,BACKUP_SIZE);
	free(backup_buf);
	return retval;
}
/**
  * @brief   :��addr����ʼ����elen���ֵ�flash
  * @param   :waddr:��ʼ��ַ,elen����������(��λΪ��)
  * @retval  :erase_fail������ʧ�ܣ���erase_ok(�����ɹ�)
**/
flash_status sector_erase_flash(uint32_t addr,uint32_t elen)
{
	uint32_t caddr = addr;
	uint32_t len=elen;
	uint32_t SectorError=0;
	flash_status retval = erase_fail;
	FLASH_EraseInitTypeDef FlashEraseInit;

	__disable_irq();        
	HAL_FLASH_Unlock();

/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_RDERR);
#endif  

/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
#endif

	while(1)
	{
		get_sector_info(caddr);
		FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       
		FlashEraseInit.Sector=flash_sector.sector;   
		FlashEraseInit.NbSectors=1;                             
		FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      
		if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
		{
			retval=erase_fail;/*����ʧ��*/
			break;
		}
		else
		{
			if(len<flash_sector.size)
			{
			  retval=erase_ok;
			  break;
			}
			else
			{
				caddr=flash_sector.satrtaddr+flash_sector.size;
				len=elen-flash_sector.size;
			}	
		}
	}
	
	//HAL_Delay(10);
	HAL_FLASH_Lock();
	__enable_irq();    
  
	return retval;
}
