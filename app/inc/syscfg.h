/**
  * @file    hybotdemo.h
  * @author  DRL
  * @version V1.0.0
  * @date    29-3-2021
  * @brief   
  *          	
**/
#ifndef _HYBOT_DEMO_H
#define _HYBOT_DEMO_H

#ifdef __cplusplus
extern "C"{
#endif

#include "tool.h"

#define HYBOTVERSION            "SandCanyonTech (c) hybot demo SW V1.00,HW V1.00 Alpha"   
#define COMPILE_TIME             __TIME__
#define COMPILE_DATE             __DATE__

#define HYBOT_VERSION_OUT()     printf("%s,%s,%s\r\n",HYBOTVERSION,COMPILE_TIME,COMPILE_DATE)

#define UPGRADEaddr             (0x080E0000)	//固件更新标识要写入的FLASH的首地址	其后四个字节
//#define UPGRADEaddr             (0x08004000)	//固件更新标识要写入的FLASH的首地址	其后四个字节
#define FLASH_FLAG_BOOT         ((uint32_t)0x4D4D3C3C)
#define FLASH_FLAG_APP          ((uint32_t)0x2B2B1A1A)
    
#define GNSS_TASK_NAME                      ("GNSS Task")
#define GNSS_TASK_PRIO                      (7)
#define GNSS_TASK_STACK_SIZE                (256)

#define AITR724_RECEIVE_TASK_NAME           ("AITR724 RECEIVE")
#define AITR724_RECEIVE_TASK_PRIO           (9)
#define AITR724_RECEIVE_TASK_STACK_SIZE     (256)

#define AITR724_INIT_TASK_NAME              ("AIR724 INIT")
#define AITR724_INIT_TASK_PRIO              (8)
#define AITR724_INIT_STACK_SIZE             (256)

#define AIR724_MSG_TASK_NAME                ("MSG out")
#define AIR724_MSG_TASK_PRIO                (10)
#define AIR724_MSG_TASK_STACK_SIZE          (256)


#define ESP32_RECEIVE_TASK_NAME              ("ESP32 recv")
#define ESP32_RECEIVE_TASK_PRIO              (3)
#define ESP32_RECEIVE_TASK_STACK_SIZE        (512)

#define ESP32_INIT_TASK_NAME                 ("ESP32 init")
#define ESP32_INIT_TASK_PRIO                 (3)
#define ESP32_INIT_TASK_STACK_SIZE           (256)

#define WIFI_SCAN_TASK_NAME                  ("Wifi scan  ")
#define WIFI_SCAN_TASK_PRIO                  (3)
#define WIFI_SCAN_TASK_STACK_SIZE            (256)

#define USART6_TASK_NAME                     ("USART6 ")
#define USART6_TASK_PRIO                     (3)
#define USART6_TASK_STACK_SIZE               (256)

#define DISK_NMEA_WRITE_TASK_NAME            ("NMEA WRITE")
#define DISK_NMEA_WRITE_TASK_PRIO            (4)
#define DISK_NMEA_WRITE_TASK_STACK_SIZE      (256)


#ifdef __cplusplus
}
#endif


#endif
