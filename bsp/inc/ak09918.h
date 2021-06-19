#ifndef _AK09918_H
#define _AK09918_H

#include "main.h"
#include "bsp_i2c.h"

#define AK09918_TEST


#define AK09918_ADDR_COMPANY_ID			        (0x00)
#define AK09918_ADDR_DEVICE_ID            (0x01)
#define AK09918_ADDR_RESERVED1            (0x02)
#define AK09918_ADDR_RESERVED2            (0x03)
#define AK09918_ADDR_STATUS1              (0x10)


/*Measurement Magnetic Data*/
#define AK09918_ADDR_X_DATA_LSB           (0x11)
#define AK09918_ADDR_X_DATA_MSB           (0x12)
#define AK09918_ADDR_Y_DATA_LSB           (0x13)
#define AK09918_ADDR_Y_DATA_MSB           (0x14)
#define AK09918_ADDR_Z_DATA_LSB           (0x15)
#define AK09918_ADDR_Z_DATA_MSB           (0x16)


#define AK09918_ADDR_STATUS2              (0x18)
#define AK09918_ADDR_CONTROL2             (0x31)
#define AK09918_ADDR_CONTROL3             (0x32)


#define AK09918_COMPANY_ID			             (0x48)
#define AK09918_DEVICE_ID                 (0x0C)

#define AK09918_ST1_READY_Pos             (0)
#define AK09918_ST1_READY_Msk             (0x01 << AK09918_ST1_READY_Pos)             /*!< 0x01*/
#define AK09918_ST1_READY                 (AK09918_ST1_READY_Msk)                     /*!< Data Ready  */


#define AK09918_ST1_DOR_Pos               (1)
#define AK09918_ST1_DOR_Msk               (0x01 << AK09918_ST1_DOR_Pos)             /*!< 0x02*/
#define AK09918_ST1_DOR                   (AK09918_ST1_DOR_Msk)                     /*!< Data Overrun  */


#define AK09918_ST2_OVERFLOW_Pos          (3)
#define AK09918_ST2_OVERFLOW_Msk          (0x01 << AK09918_ST2_OVERFLOW_Pos)             /*!< 0x08*/
#define AK09918_ST2_OVERFLOW              (AK09918_ST2_OVERFLOW_Msk)                     /*!< Magnetic sensor overflow  */


#define AK09918_CTRL3_SRST_Pos             (0)
#define AK09918_CTRL3_SRST_Msk             (0x01 << AK09918_CTRL3_SRST_Pos)             /*!< 0x01*/
#define AK09918_CTRL3_SRST                 (AK09918_CTRL3_SRST_Msk)                     /*!<  Soft reset  */


#define AK09918_PWD_MODE								     				(0x00)/*Power-down mode*/
#define AK09918_SINGLE_MODE														(0x01)/*Single measurement mode*/
#define AK09918_CONTINUOUS_MODE1	        (0x02)/*Continuous measurement mode 1*/
#define AK09918_CONTINUOUS_MODE2	        (0x04)/*Continuous measurement mode 2*/
#define AK09918_CONTINUOUS_MODE3	        (0x06)/*Continuous measurement mode 3*/
#define AK09918_CONTINUOUS_MODE4	        (0x08)/*Continuous measurement mode 4*/


void bsp_Ak09918_init(void);
void AK09918_ID_read(void);
#endif /*_AK09918_H*/



