

/**
  * @file    sd_fatfs_test.c
  * @author  DRL
  * @version V1.0.0
  * @date    26-February-2020
  * @brief   
  *          	
**/
#include "bsp_fatfs_test.h"

#define BSP_FATFS_TEST1

#ifdef BSP_FATFS_TEST
extern disk_file_info_t disk_file_info;

static __align(4) uint8_t ReadBuffer[2048]={0};         
static __align(4) uint8_t WriteBuffer1[] ="$GNGGA,000003.300,,,,,0,00,50.00,,,,,,*4D\
$GNRMC,000003.300,V,,,,,,,181015,,,*11\
$GNATT,000003.300,0,0.00,0.00,0.00,90.00,90.00,180.00*6F\
$PBSOL,1,2015,10,18,0,0,3300,3350,1867,3300,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,1,10,10,10,0,0,0\
$PSNSR,23,3350,345,-13,93,-5618,8182,-3302\
$GNGGA,000003.350,,,,,0,00,50.00,,,,,,*48\
$GNRMC,000003.350,V,,,,,,,181015,,,*14\
$GNATT,000003.350,0,0.00,0.00,0.00,90.00,90.00,180.00*6A\
$PBSOL,1,2015,10,18,0,0,3350,3400,1867,3350,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,1,10,10,10,0,0,0\
$PSNSR,23,3400,364,-21,50,-5620,8181,-3293\
$GNGGA,000003.400,,,,,0,00,50.00,,,,,,*4A\
$GNRMC,000003.400,V,,,,,,,181015,,,*16\
$GNATT,000003.400,0,0.00,0.00,0.00,90.00,90.00,180.00*68\
$PBSOL,1,2015,10,18,0,0,3400,3450,1867,3400,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,1,10,10,10,0,0,0\
$PSNSR,23,3450,353,-13,82,-5605,8188,-3301\r\n"; 





void MMC_fatfs_test(void)
{
 uint8_t cnt=0;
uint32_t offset=0;
FIL_DEV_op_t *table=file_device_table_get();
disk_fatfs_creat_file("1B002900095053424E3534200112201508100.txt");
  disk_fatfs_creat_file("1B002900095053424E3534200112201508100(sdm).txt");

printf("%s and %s write start,%f\r\n",table[FIL_SDM].path,table[FIL_NMEA].path,get_cpu_time_us());
for(cnt=0;cnt<10;cnt++)
{
  offset=Get_file_size(FIL_NMEA);
  disk_fatfs_write(FIL_NMEA,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));
}
printf("%s writing,%f,%ld\r\n",table[FIL_NMEA].path,get_cpu_time_us(),Get_file_size(FIL_NMEA));

for(cnt=0;cnt<20;cnt++)
{
  offset=Get_file_size(FIL_SDM);
  disk_fatfs_write(FIL_SDM,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));
}
printf("%s write finish,%f,%ld\r\n",table[FIL_SDM].path,get_cpu_time_us(),Get_file_size(FIL_SDM));


for(cnt=0;cnt<6;cnt++)
{
  offset=Get_file_size(FIL_NMEA);
  disk_fatfs_write(FIL_NMEA,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));
}
printf("%s writing,%f,%ld\r\n",table[FIL_NMEA].path,get_cpu_time_us(),Get_file_size(FIL_NMEA));


for(cnt=0;cnt<40;cnt++)
{
  offset=Get_file_size(FIL_SDM);
  disk_fatfs_write(FIL_SDM,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));
}
printf("%s write finish,%f,%ld\r\n",table[FIL_SDM].path,get_cpu_time_us(),Get_file_size(FIL_SDM));
}

void MMC_fatfs_rename_test(void)
{
  char *sdm_name="12345(sdm).txt";
  uint32_t offset=0;
  disk_fatfs_write(FIL_SDM,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));

  disk_fatfs_rename(FIL_SDM,(const char*)disk_file_info.SDM_name,sdm_name);

  offset=Get_file_size(FIL_SDM);

  disk_fatfs_write(FIL_SDM,offset,WriteBuffer1,strlen((const char *)WriteBuffer1));
}
#endif
