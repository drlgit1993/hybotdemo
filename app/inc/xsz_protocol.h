#ifndef _XSZ_PROTOCOL_H
#define _XSZ_PROTOCOL_H

#include "stm32f4xx.h"
#include "crc16.h"
#include "bsp_i2c.h"
#include "bsp_disk_fatfs.h"

#ifdef __cplusplus
extern "C"{
#endif

///*#define EEPROM_PAGESIZE 	             (64)*/
//#define PRI_SERVER_CONFIG_ADDR         (0)
//#define PRI_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

//#define MSG_SERVER_CONFIG_ADDR         ((PRI_SERVER_CONFIG_ADDR+PRI_SERVER_CONFIG_SIZE)/EEPROM_PAGESIZE)
//#define MSG_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

//#define OTA_SERVER_CONFIG_ADDR         ((MSG_SERVER_CONFIG_ADDR+MSG_SERVER_CONFIG_SIZE)/EEPROM_PAGESIZE)
//#define OTA_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

//#define RTCM_SERVER_CONFIG_ADDR        ((OTA_SERVER_CONFIG_ADDR+OTA_SERVER_CONFIG_SIZE)/EEPROM_PAGESIZE)
//#define RTCM_SERVER_CONFIG_SIZE        (2*EEPROM_PAGESIZE)

/*#define EEPROM_PAGESIZE 	             (64)*/
#define PRI_SERVER_CONFIG_ADDR         (0)
#define PRI_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

#define MSG_SERVER_CONFIG_ADDR         ((PRI_SERVER_CONFIG_ADDR+PRI_SERVER_CONFIG_SIZE))
#define MSG_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

#define OTA_SERVER_CONFIG_ADDR         ((MSG_SERVER_CONFIG_ADDR+MSG_SERVER_CONFIG_SIZE))
#define OTA_SERVER_CONFIG_SIZE         (EEPROM_PAGESIZE)

#define RTCM_SERVER_CONFIG_ADDR        ((OTA_SERVER_CONFIG_ADDR+OTA_SERVER_CONFIG_SIZE))
#define RTCM_SERVER_CONFIG_SIZE        (2*EEPROM_PAGESIZE)

uint8_t Is_usb_Open(void);
void Protocol_Parsing(const void *psrc,uint16_t len);

#ifdef __cplusplus
}
#endif


#endif
