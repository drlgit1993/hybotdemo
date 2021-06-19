#ifndef _DISK_TASK_H
#define _DISK_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "bsp_usart6.h"
#include "bsp_disk_fatfs.h"
#include "bsp_rng.h"




#define DISK_FILE_NAME_MAX (64)
#define DEVICE_IMEI_MAX    (32)
#define DISK_EVENT_NMEA_WRITE   (1<<0)



typedef struct
{
 uint8_t IMEI[DEVICE_IMEI_MAX];
	char NMEA_name[DISK_FILE_NAME_MAX];
 uint8_t valid;
}device_info_t;


void Disk_Task_Create(void);
void disk_nmea_EnQueue(void);


#ifdef __cplusplus
}
#endif

#endif
