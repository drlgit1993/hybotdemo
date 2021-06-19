#include "lsm6dsl_task.h"
#define BOOT_TIME    500
#define TX_BUF_DIM          128

#define LSM6DSL_CFG_CHECK    (0)

static stmdev_ctx_t dev_ctx;
static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;


static void lsm6dsl_Pin_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};

   LSM6DSL_INT1_CLK_ENABLE();

   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
   GPIO_InitStruct.Pin = LSM6DSL_INT1_PIN;
   HAL_GPIO_Init(LSM6DSL_INT1_PORT, &GPIO_InitStruct); 

   HAL_NVIC_SetPriority(LSM6DSL_INT1_EXTI_IRQ, 5, 0);
   HAL_NVIC_EnableIRQ(LSM6DSL_INT1_EXTI_IRQ);
}

static int32_t platform_write(void *handle,uint8_t reg,const uint8_t* bufp,uint16_t len)
{
   int32_t rslt=0;
   rslt=bsp_spi_write(handle,reg,bufp,len);
   return rslt;
}

static int32_t platform_read(void *handle,uint8_t reg,uint8_t* bufp,uint16_t len)
{
   int32_t rslt=0;
   bsp_spi_read(handle,reg,bufp,len);
   return rslt;
}

static void platform_delay(uint32_t ms)
{
   HAL_Delay(ms);
}

#if (1 == LSM6DSL_CFG_CHECK)
static void LSM6DSL_test(void)
{
   uint8_t i=0,reg_data=5,len=0;
   uint8_t reg_addr[]={0x01,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0d,0x0e,0x0f, \
                       0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c, \
                       0x1d,0x1e,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a, \
                       0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, \
                       0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x49,0x4a, \
                       0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x58,0x59, \
                       0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x66,0x67,0x68,0x69,0x6a,0x6b,0x73,0x74,0x75};


			len=sizeof(reg_addr);
			for(i=0;i<len;i++)
			{
							lsm6dsl_read_reg(&dev_ctx,reg_addr[i],&reg_data,1);
							USART6_printf("reg_addr[%#x]:%#x\r\n",reg_addr[i],reg_data);
			}

}
#endif

static uint8_t LSM6DSL_init(void)
{
  uint8_t lsm6dsl_id=0,rst=0;
#if (1 == LSM6DSL_CFG_CHECK)
		lsm6dsl_odr_xl_t x1_rate;
		lsm6dsl_odr_g_t gy_data;
		lsm6dsl_fs_xl_t xl_full_scale;
		lsm6dsl_fs_g_t gy_full_scale;
		lsm6dsl_input_composite_t  xl_lp2_bandwidth;
		lsm6dsl_lpf1_sel_g_t gy_band_pass;
  lsm6dsl_bw0_xl_t xl_filter_analog;
#endif

  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.delay = platform_delay;
  dev_ctx.handle = NULL;

  dev_ctx.interface=LSM6DSL_SPI_INTF;
  dev_ctx.dummy_byte=1;
  /*Wait sensor boot time*/
  platform_delay(BOOT_TIME);
 
  /*check device id*/
  lsm6dsl_device_id_get(&dev_ctx,&lsm6dsl_id);

  if (LSM6DSL_ID != lsm6dsl_id)
		{
							USART6_printf("ERROR:not LSM6DSL Device,%#x!\r\n",lsm6dsl_id);
       return 1;
		}

#if (1==LSM6DSL_CFG_CHECK)
  LSM6DSL_test();
#endif

  /*restore default configuration*/
  lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);

  do 
  {
      lsm6dsl_reset_get(&dev_ctx, &rst);
  } while (rst);
  
  /* Enable Block Data Update */
  lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);


  /* Set Output Data Rate */
  lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_12Hz5);


  lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_12Hz5);

  /* Set full scale */
  lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_2g);

  lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_2000dps);
  
  /* Configure filtering chain(No aux interface) */
  /* Accelerometer - analog filter */
  lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);

  /* Accelerometer - LPF1 path ( LPF2 not used )*/
  //lsm6dsl_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSL_XL_LP1_ODR_DIV_4);

  /* Accelerometer - LPF1 + LPF2 path */
  lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

  /* Accelerometer - High Pass / Slope path */
  //lsm6dsl_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
  //lsm6dsl_xl_hp_bandwidth_set(&dev_ctx, LSM6DSL_XL_HP_ODR_DIV_100);
  /* Gyroscope - filtering chain */
  lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);


#if (1 == LSM6DSL_CFG_CHECK)
  lsm6dsl_block_data_update_get(&dev_ctx,&rst);
  lsm6dsl_xl_data_rate_get(&dev_ctx, &x1_rate);
  lsm6dsl_gy_data_rate_get(&dev_ctx, &gy_data);
  lsm6dsl_xl_full_scale_get(&dev_ctx, &xl_full_scale);
  lsm6dsl_gy_full_scale_get(&dev_ctx, &gy_full_scale);
  lsm6dsl_xl_filter_analog_get(&dev_ctx, &xl_filter_analog);
  lsm6dsl_xl_lp2_bandwidth_get(&dev_ctx, &xl_lp2_bandwidth);
  lsm6dsl_gy_band_pass_get(&dev_ctx, &gy_band_pass);
#endif

  return 0;
}
typedef struct
{
  int16_t raw_acc_x;
  int16_t raw_acc_y;
  int16_t raw_acc_z;
  int16_t raw_ang_x;
  int16_t raw_ang_y;
  int16_t raw_ang_z;
  int16_t temperature;
}lsm6dsl_raw_data_t;


uint8_t lsm6dsl_acceleration_raw_get_polling(lsm6dsl_raw_data_t *praw_data)
{
    lsm6dsl_status_reg_t status;
    lsm6dsl_status_reg_get(&dev_ctx, &status);

    if (status.xlda) 
    {
							memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
							lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
							praw_data->raw_acc_x=data_raw_acceleration[0];
							praw_data->raw_acc_y=data_raw_acceleration[1];
							praw_data->raw_acc_z=data_raw_acceleration[2];
       return 1;
    }
    return 0;
}

uint8_t lsm6dsl_angular_raw_get_polling(lsm6dsl_raw_data_t *praw_data)
{
    lsm6dsl_status_reg_t status;
    lsm6dsl_status_reg_get(&dev_ctx, &status);

    if (status.gda) 
    {
							memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
							lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
							praw_data->raw_ang_x=data_raw_angular_rate[0];
							praw_data->raw_ang_y=data_raw_angular_rate[1];
							praw_data->raw_ang_z=data_raw_angular_rate[2];
       return 1;
    }
    return 0;
}

uint8_t lsm6dsl_temperature_raw_get_polling(lsm6dsl_raw_data_t *praw_data)
{
    lsm6dsl_status_reg_t status;
    lsm6dsl_status_reg_get(&dev_ctx, &status);

    if (status.tda) 
    {
							data_raw_temperature= 0x00;
							lsm6dsl_temperature_raw_get(&dev_ctx, &data_raw_temperature);
       praw_data->temperature=data_raw_temperature;
       return 1;
    }
    return 0;
}

static void lsm6dsl_polling_test(void)
{
    lsm6dsl_raw_data_t raw_data;
				float acceleration_mg[3];
				float angular_rate_mdps[3];
				float temperature_degC;
				char tx_buffer[TX_BUF_DIM];

    if(lsm6dsl_acceleration_raw_get_polling(&raw_data))
    {
       acceleration_mg[0] = lsm6dsl_from_fs2g_to_mg(raw_data.raw_acc_x);
       acceleration_mg[1] = lsm6dsl_from_fs2g_to_mg(raw_data.raw_acc_y);
       acceleration_mg[2] = lsm6dsl_from_fs2g_to_mg(raw_data.raw_acc_z);
       sprintf(tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
       USART6_printf(tx_buffer);
    }

    if (lsm6dsl_angular_raw_get_polling(&raw_data)) 
    {
       angular_rate_mdps[0] = lsm6dsl_from_fs2000dps_to_mdps(raw_data.raw_ang_x);
       angular_rate_mdps[1] = lsm6dsl_from_fs2000dps_to_mdps(raw_data.raw_ang_y);
       angular_rate_mdps[2] = lsm6dsl_from_fs2000dps_to_mdps(raw_data.raw_ang_z);
       sprintf((char *)tx_buffer,"Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
       USART6_printf(tx_buffer);    
    }

    if (lsm6dsl_temperature_raw_get_polling(&raw_data)) 
    {
      temperature_degC = lsm6dsl_from_lsb_to_celsius(raw_data.temperature );
      sprintf((char *)tx_buffer, "Temperature [degC]:%6.2f\r\n",temperature_degC );
      USART6_printf(tx_buffer);
    }
}

void lsm6dsl_Task_Create(void)
{
   MX_SPI1_Init();
   lsm6dsl_Pin_Init();
  
   if(LSM6DSL_init())
	  {
				   return ;
	  }
   lsm6dsl_polling_test();
}

void LSM6DSL_INT1_IRQHandler(void)
{
	  if(__HAL_GPIO_EXTI_GET_IT(LSM6DSL_INT1_PIN) != RESET) 
	  {
		     __HAL_GPIO_EXTI_CLEAR_IT(LSM6DSL_INT1_PIN);     
	  }  
}

