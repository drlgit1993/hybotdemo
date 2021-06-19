/**
  * @file    bsp_disk_fatfs.c
  * @author  DRL
  * @version V1.0.0
  * @date    24-February-2020
  * @brief   
  *          	
**/
#include "bsp_disk_fatfs.h"
#include "log.h"

const char *fatfs_disk_err[20]=
{
   "Succeeded",
   "A hard error occurred in the low level disk I/O layer",
   "Assertion failed",
   "The physical drive cannot work",
   "Could not find the file",
   "Could not find the path ",
   "The path name format is invalid",
   "Access denied due to prohibited access or directory full",
   "Access denied due to prohibited access",
   "The file/directory object is invalid",
   "The physical drive is write protected",
   "The logical drive number is invalid",
   "The volume has no work area",
   "There is no valid FAT volume",
   "The f_mkfs() aborted due to any problem",
   "Could not get a grant to access the volume within defined period",
   "The operation is rejected according to the file sharing policy",
   "LFN working buffer could not be allocated",
   "Number of open files > FF_FS_LOCK",
   "Given parameter is invalid",
};
char disk_Path[4]; 

FIL fnew;													/* 文件对象 */
FIL fNMEA;													/* 文件对象 */
FIL fSDM;													/* 文件对象 */
FATFS fs;													/* FatFs文件系统对象 */

uint8_t table_len=0;

FIL_DEV_op_t FIL_DEV_TABLE[3]=
{
 {FIL_ERR,0,0},
 {FIL_ERR,0,0},
 {FIL_ERR,0,0}
};

FIL_DEV_op_t *file_device_table_get(void)
{
   return (FIL_DEV_TABLE);
}
/**
* @brief  获取sd卡容量信息
* @param  sd_cap:sd卡容量结构体指针
* @retval 文件的大小
*/
uint8_t disk_fatfs_capacity_get(disk_capatity_t *capatity)
{
	  FRESULT res;               	  

 	 DWORD fre_clust;
	  FATFS *FS;
		
	  res=f_getfree("0:",&fre_clust,&FS);	//必须是根目录，选择磁盘0
		
	  if(FR_OK ==res)
		 {
			    capatity->total=(uint32_t)(((FS->n_fatent-2)*FS->csize)>>11);
			    capatity->available=(uint32_t)((fre_clust*FS->csize)>>11);
			    return 0x00;
		 }
			
		 return 0x01;
}

/**
* @brief  卡及fatfs文件系统初始化
* @param  none
* @retval none
*/
uint8_t bsp_disk_fatfs_init(void)
{
   FRESULT res; 

   uint8_t i=0;

   for(i=0;i<3;i++)
			{
			FIL_DEV_TABLE[i].state=FIL_ERR;
			}

	  res = f_mount(&fs,"0:",1);
	  if(res == FR_NO_FILESYSTEM)
	  {
		     res=f_mkfs("0:",0,0,FF_MAX_SS);		
		     if(res == FR_OK)
		     {
			        res = f_mount(NULL,"0:",1);				
			        res = f_mount(&fs,"0",1);
		     }
		     else
		     {
			        CONSLOE_LOG(LOG_LVL_ERROR,"format card fail!");
           return 1;
		     }
	  }
	
   else if(res!=FR_OK)
   {
       CONSLOE_LOG(LOG_LVL_ERROR,"mount file system fail!");
       return 2;
   }
   return 0;
}
void bsp_disk_fatfs_Deinit(void)
{
   FRESULT res; 

	  res = f_mount(NULL,"0:",1);	/*取消挂载*/				

   if(res == FR_OK)
	  {
			    CONSLOE_LOG(LOG_LVL_DEBUG,"Demount card success!");
   }
}

void disk_fatfs_rename(FIL_DEV_t dev,const TCHAR* path_old,	const TCHAR* path_new	)
{

   if(FIL_READY !=FIL_DEV_TABLE[dev].state)
   {
       return ;
   }

 // f_close(FIL_DEV_TABLE[dev].file);
  memset(FIL_DEV_TABLE[dev].path,0x00,64);
  strcpy(FIL_DEV_TABLE[dev].path,path_new);
  f_rename(path_old,path_new);
//  res = f_open(FIL_DEV_TABLE[dev].file, FIL_DEV_TABLE[dev].path, FA_READ|FA_WRITE);
//    
//    if ( res == FR_OK )
//	   {
//      FIL_DEV_TABLE[dev].state=FIL_READY;
//	   // 	printf("reopen %s success!\r\n",FIL_DEV_TABLE[dev].path);
//	   }
//				else if(res==FR_EXIST)
//				{
//			//	printf("reopen  %s  exist\r\n",FIL_DEV_TABLE[dev].path);
//				}
//	   else
//	   {		
//		    printf("reopen  %s  fail:%d\r\n",FIL_DEV_TABLE[dev].path,res);
//	   }

  //f_sync(FIL_DEV_TABLE[dev].file);
}
/**
* @brief  使用fatfs创建文件
* @param  path：文件路径及文件名;
* @retval none
*/
FRESULT disk_fatfs_creat_file(void *path,BYTE mode)
{
	  FRESULT res;               	 
	  char *ppath=(char*)path;
   res = f_open(&fnew, (const char*)ppath, mode);
    
   if ( res == FR_OK )
	  {
       memcpy(FIL_DEV_TABLE[table_len].path,path,strlen(path));
       FIL_DEV_TABLE[table_len].state=FIL_READY;
       table_len ++;	
       f_close(&fnew);
       CONSLOE_LOG(LOG_LVL_DEBUG,"create  %s  success",ppath);
	  }
			else if(res==FR_EXIST)
			{
				   CONSLOE_LOG(LOG_LVL_ERROR,"create  %s  exist",ppath);
			}
	  else
	  {		
		     CONSLOE_LOG(LOG_LVL_ERROR,"create  %s  fail:%d",ppath,res);
	  } 
   return res;
}

void disk_close(FIL_DEV_t dev)
{
   f_close(FIL_DEV_TABLE[dev].file);
}

void Disk_Busy_Set(void)
{
   for (uint8_t index=0;index<table_len;index++)
  {
       FIL_DEV_TABLE[index].state=FIL_BUSY;
  }

}
uint8_t IS_Disk_Ready(FIL_DEV_t dev)
{
   return (FIL_READY ==FIL_DEV_TABLE[dev].state);
}

/**
* @brief  使用fatfs向卡写数据
* @param  path：文件路径及文件名;offset:写指针偏移量;WriteBuffer：数据指针;write_size：待写入的数据长度.
* @retval 写入的文件大小
*/
UINT disk_fatfs_write(FIL_DEV_t dev,uint8_t *WriteBuffer,uint32_t write_size)
{
	  FRESULT res;               	  /* 文件操作结果 */
	  UINT size;

   if(FIL_DEV_TABLE[dev].state != FIL_READY)
   {
       return 0;
   }

	  res = f_open(&fnew,FIL_DEV_TABLE[dev].path, FA_OPEN_APPEND|FA_WRITE|FA_READ );
	  if ( res == FR_OK )
	  {	
		     res=f_write(&fnew,WriteBuffer,write_size,&size);
		
       if(res != FR_OK)
       {
           CONSLOE_LOG(LOG_LVL_ERROR,"ERROR:%s write data fail,%s,%d",FIL_DEV_TABLE[dev].path,fatfs_disk_err[res],size);

		  	      size=0x00;
       } 
	  }
	  else
	  {	
		    size=0x00;
		
		    CONSLOE_LOG(LOG_LVL_ERROR,"ERROR:write %s open  fail,%s",FIL_DEV_TABLE[dev].path,fatfs_disk_err[res]);
	  }
	
			/* 不再读写，关闭文件 */
   f_close(&fnew);
	 //CONSLOE_LOG(LOG_LVL_INFO,"INFO:%s closed,%d,%d",FIL_DEV_TABLE[dev].path,write_size,fnew.obj.objsize);
	return size;
}

/**
* @brief  使用fatfs读取卡的数据
* @param  path：文件路径及文件名;offset:读指针偏移量;WriteBuffer：数据缓存指针;read_size：待读取的数据长度.
* @retval 读取的数据大小
*/
UINT disk_fatfs_read(FIL_DEV_t dev,uint32_t offset,uint8_t *ReadBuffer,uint32_t read_size)
{
	  FRESULT res;               	  
	  UINT size;
   FSIZE_t ptr=0;

	  if(FIL_READY !=FIL_DEV_TABLE[dev].state)
   {
       return 0;
   }

   res = f_open(&fnew, FIL_DEV_TABLE[dev].path, FA_READ); 
	  if(res == FR_OK)
	  {
       ptr=fnew.fptr;
	      f_lseek(&fnew,offset);
		     res = f_read(&fnew, ReadBuffer, read_size, &size); 
		
       if(res != FR_OK)
       {
			        size=0x00;
           CONSLOE_LOG(LOG_LVL_ERROR,"ERROR:%s read data fail,%s",FIL_DEV_TABLE[dev].path,fatfs_disk_err[res]);
       }		
       f_lseek(&fnew,ptr);	
       f_close(&fnew);	
   }
   else
   {
       size=0x00;
       CONSLOE_LOG(LOG_LVL_ERROR,"ERROR:read %s open file  fail,%s",FIL_DEV_TABLE[dev].path,fatfs_disk_err[res]);
   }
   
	  return size;
}


