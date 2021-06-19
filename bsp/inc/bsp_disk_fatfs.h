/**
  * @file    bsp_sd_fatfs.h
  * @author  DRL
  * @version V1.0.0
  * @date    24-February-2020
  * @brief   
  *          	
**/
#ifndef _BSP_DISK_FATFS_H
#define _BSP_DISK_FATFS_H

#ifdef __cplusplus
extern "C"{
#endif


#include "diskio.h"



typedef enum
{
  FIL_NMEA,
  FIL_LIST
}FIL_DEV_t;

typedef enum
{
  FIL_READY,
  FIL_ERR,
  FIL_BUSY
}FIL_STATE_t;

typedef struct
{
 FIL_STATE_t state;
 FIL *file;
 char path[64];

}FIL_DEV_op_t;


typedef struct
{
  uint16_t total;
	uint16_t available;
}disk_capatity_t;

/**
* @brief  获取sd卡容量信息
* @param  sd_cap:sd卡容量结构体指针
* @retval 文件的大小
*/
uint8_t disk_fatfs_capacity_get(disk_capatity_t *capatity);
/**
* @brief  SD卡fatfs文件系统初始化
* @param  none
* @retval none
*/
uint8_t bsp_disk_fatfs_init(void);

void bsp_disk_fatfs_Deinit(void);


/**
* @brief  系统文件夹初始化
* @param  
* @retval 
*/
uint8_t file_dir_init(void *path);
 
void Disk_Busy_Set(void);

void bsp_fkfs(void);


UINT disk_fatfs_read(FIL_DEV_t dev,uint32_t offset,uint8_t *ReadBuffer,uint32_t read_size);

UINT disk_fatfs_write(FIL_DEV_t dev,uint8_t *WriteBuffer,uint32_t write_size);

FRESULT disk_fatfs_creat_file(void *path,BYTE mode);
void disk_close(FIL_DEV_t dev);

DWORD Get_file_size(FIL_DEV_t dev);

FIL_DEV_op_t *file_device_table_get(void);
void disk_fatfs_rename(FIL_DEV_t dev,const TCHAR* path_old,	const TCHAR* path_new	);
uint8_t IS_Disk_Ready(FIL_DEV_t dev);

#ifdef __cplusplus
}
#endif

#endif
