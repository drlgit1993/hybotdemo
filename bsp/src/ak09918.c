#include "ak09918.h"

void AK09918_ID_read(void)
{
  uint8_t devc_id=0x00,cmpy_id=0x00,reg_addr=0x00;

		reg_addr=AK09918_ADDR_DEVICE_ID;
  AK09918_Read(reg_addr,&devc_id,1);
 
		reg_addr=AK09918_ADDR_COMPANY_ID;
		AK09918_Read(reg_addr,&cmpy_id,1);

  printf("device id:%#x,company id:%#x\r\n",devc_id,cmpy_id);
}
static void AK09918_mode_get(void)
{
			uint8_t mode=0x00,reg_addr=AK09918_ADDR_CONTROL2;
			AK09918_Read(reg_addr,&mode,1);
			printf("%#x\r\n",mode);
}
/*
 AK09918_PWD_MODE	:Power-down mode(0x00)
 AK09918_SINGLE_MODE:Single measurement mode(0x01)
 AK09918_CONTINUOUS_MODE1:Continuous measurement mode (0x02)
 AK09918_CONTINUOUS_MODE2:Continuous measurement mode (0x04)
 AK09918_CONTINUOUS_MODE3:Continuous measurement mode (0x06)
 AK09918_CONTINUOUS_MODE4:Continuous measurement mode (0x08)
*/
static void AK09918_mode_write(uint8_t mode)
{
  uint8_t mode_t=mode,reg_addr=AK09918_ADDR_CONTROL2;
  AK09918_Write(reg_addr,&mode_t,1);
}

void bsp_Ak09918_init(void)
{
   bsp_I2C2_Init();
   AK09918_ID_read();

#ifdef AK09918_TEST
   printf("AK09918 default mode:");
   AK09918_mode_get();
   printf("AK09918 signal mode:");
   AK09918_mode_write(AK09918_SINGLE_MODE);
   AK09918_mode_get();
			printf("AK09918 continuous mode1:");
   AK09918_mode_write(AK09918_CONTINUOUS_MODE1);
   AK09918_mode_get();
			printf("AK09918 continuous mode2:");
   AK09918_mode_write(AK09918_CONTINUOUS_MODE2);
   AK09918_mode_get();
			printf("AK09918 continuous mode3:");
   AK09918_mode_write(AK09918_CONTINUOUS_MODE3);
   AK09918_mode_get();
			printf("AK09918 continuous mode4:");
   AK09918_mode_write(AK09918_CONTINUOUS_MODE4);
   AK09918_mode_get();
			printf("AK09918 power down mode:");
   AK09918_mode_write(AK09918_PWD_MODE);
   AK09918_mode_get();
#endif

}
