/**
  * @file    bsp_fatfs_test.h
  * @author  DRL
  * @version V1.0.0
  * @date    24-October-2020
  * @brief   
  *          	
**/

#ifndef _BSP_FATFS_TEST_H
#define _BSP_FATFS_TEST_H


#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"
#include "bsp_disk_fatfs.h"


void MMC_fatfs_test(void);

void MMC_fatfs_rename_test(void);

#ifdef __cplusplus
}
#endif


#endif
