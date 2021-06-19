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
  * @brief    :设置扇区地址，扇区首地址，扇区大小
  * @param    :sector：扇区地址，satrtaddr：扇区首地址，size：扇区大小
  * @retval   :none
**/
static void flash_sector_info_set(uint16_t sector,uint32_t satrtaddr,uint32_t size)
{
  flash_sector.sector=sector;
  flash_sector.satrtaddr=satrtaddr;
  flash_sector.size=size;
}

/**
  * @brief    :获取地址addr所在的flash扇区，扇区首地址，扇区大小
  * @param    :addr：flash地址
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
  * @brief  从addr处读取一个字(4字节)
  * @param  addr：读取的地址
  * @retval 读取到的数据
**/
uint32_t read_oneword_flash(uint32_t addr)
{
  return *(__IO uint32_t*)addr;
}

/**
  * @brief    :从addr处开始读len个字的数据
  * @param    :addr:读取的首地址，pbuff：读取到的数据缓存，len:需要读取的数据长度/sizeof(uint32_t)
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
  * @param    :addr:读取的首地址，pbuff：读取到的数据缓存，len:需要读取的数据长度
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
  * @brief    :从addr处开始读出len个半字的数据
  * @param    :addr:待读取的首地址，pbuff：数据缓存，len:需要读取的数据长度
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
  * @brief   :从waddr写wlen字节的数据
  * @param   :waddr:首地址，pbuff：数据指针,wlen:待写入的数据长度(单位为字)
  * @retval  :write_ok(写入成功),write_fail(写入失败)
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
      return write_fail;	/*写入失败*/
    }
    caddr += 4;
    pbuff ++;
  }
   return write_ok;
 }

/**
  * @brief    :直接写入数据到flash,写入区域已经被擦除.
  * @param    :waddr:起始地址, pbuff:数据指针,wlen：写入的数据长度（单位为字）
  * @retval   :write_ok(写入成功),write_fail(写入失败),addr_err(写入的地址非法)
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
  * @brief    :写入区域先擦除，再写入数据。
  * @param    :waddr:写入区域首地址, pbuf:数据缓存,wlen:写入的数据长度（单位为字）
  * @retval   :write_ok(写入数据成功)，addr_err(写入的地址非法），erase_fail（擦除数据失败），write_fail（数据写入失败)
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

	__disable_irq();        /*禁止中断*/
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
	/*擦除待写入的区域*/
	while(caddr<Wendaddr)
	{
   
		if((*(uint32_t*)caddr)!=0XFFFFFFFF)/*当前地址非空，擦除当前地址所在扇区*/
		{
			get_sector_info(caddr);

			FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       
			FlashEraseInit.Sector=flash_sector.sector;   
			FlashEraseInit.NbSectors=1;                             
			FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      
			if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
			{
				return erase_fail;/*擦除失败*/
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

    __enable_irq();      /*开启中断*/

    return retval;
}

/**
  * @brief   :先备份BACKUP_SIZE字大小的数据，将待写入的数据插入备份数据中，然后擦除整个扇区，最后将所有备份的数据写入扇区。
  * @param   :waddr:起始地址,pbuf:待写入的数据,wlen：写入的数据长度(单位为字)
  * @retval  :write_ok(写入数据成功)，addr_err(写入的地址非法），erase_fail（擦除数据失败），write_fail（数据写入失败)
**/
flash_status backup_write_flash(uint32_t waddr,uint32_t *pbuf,uint16_t wlen)
{
	flash_status retval = write_fail;
	uint32_t waddrOffset=0;
	uint32_t *backup_buf=NULL;
	uint32_t *tpbuf=NULL;

	get_sector_info(waddr);
	waddrOffset=(waddr-flash_sector.satrtaddr)>>2;/*获取待写入首地址与本扇区首地址的偏移量,单位为字*/
	backup_buf=(uint32_t*)malloc(sizeof(uint32_t)*BACKUP_SIZE);
	tpbuf=backup_buf+waddrOffset;
	read_mulWord_flash(flash_sector.satrtaddr,backup_buf,BACKUP_SIZE);/*备份数据*/

	while(wlen--)/*将待写入的数据插入备份区域*/
	{
		*tpbuf++ = *pbuf++;
	}

	retval = nobackup_write_flash(flash_sector.satrtaddr,backup_buf,BACKUP_SIZE);
	free(backup_buf);
	return retval;
}
/**
  * @brief   :从addr处开始擦除elen个字的flash
  * @param   :waddr:起始地址,elen：擦除长度(单位为字)
  * @retval  :erase_fail（擦除失败），erase_ok(擦除成功)
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
			retval=erase_fail;/*擦除失败*/
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
