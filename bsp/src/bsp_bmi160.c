#include "bsp_bmi160.h"



struct bmi160_dev bmi160_sensor;
struct bmi160_int_settg bmi160_interrupt;

void  bmi160_get_accel_data(struct bmi160_sensor_data *accel) 
{
  bmi160_get_sensor_data(BMI160_ACCEL_SEL, accel, NULL, &bmi160_sensor);
}
void  bmi160_get_gyro_data(struct bmi160_sensor_data *gyro)  
{
   bmi160_get_sensor_data(BMI160_GYRO_SEL, NULL, gyro, &bmi160_sensor);
}

void  bmi160_get_accel_gyro_data(struct bmi160_sensor_data *accel,struct bmi160_sensor_data *gyro)
{
   bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), accel, gyro, &bmi160_sensor);
}

static int8_t bsp_bmi160_config(void)
{
	int8_t rslt = BMI160_OK;

	/* Accel Output data rate */
//	bmi160_sensor.accel_cfg.odr=BMI160_ACCEL_ODR_1600HZ;

	bmi160_sensor.accel_cfg.odr=BMI160_ACCEL_ODR_0_78HZ;

	/* Accel Range setting*/
	bmi160_sensor.accel_cfg.range=BMI160_ACCEL_RANGE_2G;

	/* Accel Bandwidth setting*/
	bmi160_sensor.accel_cfg.bw=BMI160_ACCEL_BW_NORMAL_AVG4;

	/* Accel power mode setting*/
	bmi160_sensor.accel_cfg.power=BMI160_ACCEL_NORMAL_MODE;


	/* Gyro Output data rate setting*/
	bmi160_sensor.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;

	/* Gyro Output Range setting*/
	bmi160_sensor.gyro_cfg.range = BMI160_GYRO_RANGE_1000_DPS;

	/* Gyro Bandwidth setting*/
	bmi160_sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

	/* Gyro power mode setting*/
	bmi160_sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE; 

	/* Set the sensor configuration */
	rslt = bmi160_set_sens_conf(&bmi160_sensor);

	return rslt;
}

 int8_t bsp_bmi160_lowpwr_config(void)
{
	int8_t rslt = BMI160_OK;

	/* Accel Output data rate */
	bmi160_sensor.accel_cfg.odr=BMI160_ACCEL_ODR_0_78HZ;
	bmi160_sensor.accel_cfg.power=BMI160_ACCEL_LOWPOWER_MODE;
	bmi160_sensor.gyro_cfg.power=BMI160_GYRO_SUSPEND_MODE;
	rslt = bmi160_set_sens_conf(&bmi160_sensor);
	return rslt;
}

 int8_t bsp_bmi160_accel_power_config(uint8_t power)
{
  int8_t rslt = BMI160_OK;

/* Select the power mode */
	bmi160_sensor.accel_cfg.power=power;

/*  Set the Power mode  */
	rslt = bmi160_set_power_mode(&bmi160_sensor);

	return rslt;
}

 int8_t bsp_bmi160_gyro_power_config(uint8_t power)
{
  int8_t rslt = BMI160_OK;

/* Select the power mode */
	bmi160_sensor.gyro_cfg.power=power;

/*  Set the Power mode  */
	rslt = bmi160_set_power_mode(&bmi160_sensor);

	return rslt;
}
  void bsp_bmi160_get_power_mode(void)
{
  struct bmi160_pmu_status pmu_status;
  bmi160_get_power_mode(&pmu_status,&bmi160_sensor);
  printf("accel:%#x,gyro:%#x\r\n",pmu_status.accel_pmu_status,pmu_status.gyro_pmu_status);
}

static int8_t bsp_bmi160_interrupt_config(void)
{

	int8_t rslt = BMI160_OK;
	bmi160_interrupt.int_channel=BMI160_INT_CHANNEL_1;/* Interrupt channel/pin 1*/

	bmi160_interrupt.int_type=BMI160_ACC_ANY_MOTION_INT;/*Choosing Any motion interrupt*/
	bmi160_interrupt.int_pin_settg.output_en=BMI160_ENABLE;
	bmi160_interrupt.int_pin_settg.output_mode=BMI160_DISABLE;
	bmi160_interrupt.int_pin_settg.output_type=BMI160_ENABLE;
	bmi160_interrupt.int_pin_settg.edge_ctrl=BMI160_DISABLE;
	bmi160_interrupt.int_pin_settg.input_en=BMI160_DISABLE;
	bmi160_interrupt.int_pin_settg.latch_dur=BMI160_LATCH_DUR_NONE;

	/* Select the Any-motion interrupt parameters */
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_en = BMI160_ENABLE;// 1- Enable the any-motion, 0- disable any-motion 
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_x = BMI160_ENABLE;// Enabling x-axis for any motion interrupt
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_y = BMI160_ENABLE;// Enabling y-axis for any motion interrupt
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_z = BMI160_ENABLE;// Enabling z-axis for any motion interrupt
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_dur = 0;// any-motion duration
	bmi160_interrupt.int_type_cfg.acc_any_motion_int.anymotion_thr = 20;// (2-g range) -> (slope_thr) * 3.91 mg, (4-g range) -> (slope_thr) * 7.81 mg, (8-g range) ->(slope_thr) * 15.63 mg, (16-g range) -> (slope_thr) * 31.25 mg 


	/* Set the Any-motion interrupt */
	rslt=bmi160_set_int_config(&bmi160_interrupt, &bmi160_sensor); /* sensor is an instance of the structure bmi160_dev  */

	return rslt;
}
static void bmi160_interrupt_config_get(void)
{
  uint8_t cfg=0x00,cfg_addr=BMI160_INT_ENABLE_0_ADDR;

	bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT1_EN0[%#x] :%#x\r\n",cfg_addr,cfg);


 cfg_addr=BMI160_INT_ENABLE_1_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT1_EN1[%#x] :%#x\r\n",cfg_addr,cfg);


  cfg_addr=BMI160_INT_ENABLE_2_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT1_EN2[%#x] :%#x\r\n",cfg_addr,cfg);


  cfg_addr=BMI160_INT_OUT_CTRL_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("EN_OUT[%#x] :%#x\r\n",cfg_addr,cfg);

  cfg_addr=BMI160_INT_LATCH_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT_LATCH[%#x] :%#x\r\n",cfg_addr,cfg);

  cfg_addr=BMI160_INT_MAP_0_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT_MAP0[%#x] :%#x\r\n",cfg_addr,cfg);


  cfg_addr=BMI160_INT_MAP_1_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT_MAP1[%#x] :%#x\r\n",cfg_addr,cfg);



  cfg_addr=BMI160_INT_MAP_2_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("INT_MAP2[%#x] :%#x\r\n",cfg_addr,cfg);

}

 void bmi160_read_id(void)
{
	uint8_t reg_addr=0x0F |0x80;

uint8_t id[3]={0},id1;

 bsp_bmi160_read(0,  reg_addr, id ,  3);

	//bmi160_get_regs(reg_addr, &id, 1, &bmi160_sensor);

	USART6_printf("BMI160 ID:%#x,%#x,%#x\r\n",id[0],id[1],id[2]);

bsp_bmi160_read(0,  reg_addr, &id1 ,  1);

USART6_printf("BMI160 ID:%#x\r\n",id1);
}
void bsp_bmi160_interrupt_get(void)
{
//	union bmi160_int_status int_status;
//	bmi160_get_int_status(BMI160_INT_STATUS_0,&int_status,&bmi160_sensor);

// bmi160_get_int_status(BMI160_INT_STATUS_2,&int_status,&bmi160_sensor);
//	printf("status0:%#x,status2:%#x\r\n",int_status.data[1],int_status.data[2]);

 bmi160_read_id();
}
static void bmi160_get_accl_cfg(void)
{
	uint8_t cfg=0x00,cfg_addr=BMI160_ACCEL_CONFIG_ADDR;

	bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("accl cfg:%#x\r\n",cfg);

 cfg_addr=BMI160_ACCEL_RANGE_ADDR;

 bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("accl range:%#x\r\n",cfg);
}

static void bmi160_get_gyr_cfg(void)
{
	uint8_t cfg=0x00,cfg_addr=BMI160_GYRO_CONFIG_ADDR;

	bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("gyr cfg:%#x\r\n",cfg);

  cfg_addr=BMI160_GYRO_RANGE_ADDR;

  bmi160_get_regs(cfg_addr, &cfg, 1, &bmi160_sensor);

	printf("gyr range:%#x\r\n",cfg);
}
static void bsp_bmi160_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   BMI160_INT1_CLK_ENABLE();

   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pin = BMI160_INT1_PIN;
   HAL_GPIO_Init(BMI160_INT1_PORT, &GPIO_InitStruct); 
   HAL_NVIC_SetPriority(BMI160_INT1_EXTI_IRQ, 5, 0);
   HAL_NVIC_EnableIRQ(BMI160_INT1_EXTI_IRQ);
}

int8_t bsp_bmi160_init(void)
{
  int8_t rslt = BMI160_OK;

  MX_SPI1_Init();
  //bsp_bmi160_Pin_Init();

	 bmi160_sensor.id=0;
 	bmi160_sensor.interface=BMI160_SPI_INTF;
	 bmi160_sensor.read=bsp_bmi160_read;
 	bmi160_sensor.write=bsp_bmi160_write;
	 bmi160_sensor.delay_ms=bsp_bmi160_delay_ms;

  rslt=bmi160_init(&bmi160_sensor);

  bmi160_read_id();

  rslt |=bsp_bmi160_config();
  bmi160_get_accl_cfg();
  bmi160_get_gyr_cfg();

//  bmi160_perform_self_test(BMI160_ACCEL_ONLY,&bmi160_sensor);
//  bmi160_perform_self_test(BMI160_GYRO_ONLY,&bmi160_sensor);




	
  USART6_printf("accel and gryo mode:suspend\t");
  bsp_bmi160_accel_power_config(BMI160_ACCEL_SUSPEND_MODE);//BMI160_ACCEL_SUSPEND_MODE,BMI160_ACCEL_NORMAL_MODE,BMI160_ACCEL_LOWPOWER_MODE
  bsp_bmi160_gyro_power_config(BMI160_GYRO_SUSPEND_MODE);//BMI160_GYRO_SUSPEND_MODE,BMI160_GYRO_NORMAL_MODE,BMI160_GYRO_FASTSTARTUP_MODE
  bsp_bmi160_get_power_mode();




 rslt = BMI160_OK;
  USART6_printf("accel  mode:low-power,gryo mode:faststartup\t");
	bsp_bmi160_accel_power_config(BMI160_ACCEL_LOWPOWER_MODE);//BMI160_ACCEL_SUSPEND_MODE,BMI160_ACCEL_NORMAL_MODE,BMI160_ACCEL_LOWPOWER_MODE
	bsp_bmi160_gyro_power_config(BMI160_GYRO_FASTSTARTUP_MODE);//BMI160_GYRO_SUSPEND_MODE,BMI160_GYRO_NORMAL_MODE,BMI160_GYRO_FASTSTARTUP_MODE
  bsp_bmi160_get_power_mode();

rslt = BMI160_OK;
	USART6_printf("accel and gryo mode:normal\t");
	rslt|=bsp_bmi160_accel_power_config(BMI160_ACCEL_NORMAL_MODE);//BMI160_ACCEL_SUSPEND_MODE,BMI160_ACCEL_NORMAL_MODE,BMI160_ACCEL_LOWPOWER_MODE
rslt|=bsp_bmi160_gyro_power_config(BMI160_GYRO_NORMAL_MODE);//BMI160_GYRO_SUSPEND_MODE,BMI160_GYRO_NORMAL_MODE,BMI160_GYRO_FASTSTARTUP_MODE

  bsp_bmi160_get_power_mode();

  bsp_bmi160_interrupt_config();
  bmi160_interrupt_config_get();
  bsp_bmi160_interrupt_get();

//  bmi160_get_regs(BMI160_STATUS_ADDR, &reg_data, 1, &bmi160_sensor);
//  printf("status[0x1B]:%#x\r\n",reg_data);

return rslt;
}

void BMI160_INT1_IRQHandler(void)
{
	  if(__HAL_GPIO_EXTI_GET_IT(BMI160_INT1_PIN) != RESET) 
	  {
		     __HAL_GPIO_EXTI_CLEAR_IT(BMI160_INT1_PIN);     
	  }  
}

