/**
  * @file    bsp_ec20.c
  * @author  DRL
  * @version V1.0.0
  * @date    3-April-2020
  * @brief   
  *          	
**/
#include	"bsp_ec20.h"



/**
* @brief  EC20ģ�鿪��/�ػ����ų�ʼ��PB2
* @param  none
* @retval none
*/
static void EC20_Power_PIN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_PWR_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_PWR_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_PWR_PORT,&GPIO_InitStruct);
}
/**
* @brief  EC20ģ���Դʹ�����ų�ʼ��PA8
* @param  none
* @retval none
*/
static void EC20_Power_EN_PIN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_PWR_EN_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_PWR_EN_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_PWR_EN_PORT,&GPIO_InitStruct);
}
/**
* @brief  EC20ģ�鸴λ���ų�ʼ��
* @param  none
* @retval none
*/
static void EC20_Reset_PIN_init(void)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_RST_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_RST_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_RST_PORT,&GPIO_InitStruct);
	
	HAL_GPIO_WritePin(EC20_RST_PORT,EC20_RST_PIN,GPIO_PIN_RESET);	
}

/**
* @brief  EC20ģ��˯�߿������ų�ʼ��PA7
* @param  none
* @retval none
*/
static void EC20_WKP_PIN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_WAKEUP_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_WAKEUP_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_WAKEUP_PORT,&GPIO_InitStruct);
	HAL_GPIO_WritePin(EC20_WAKEUP_PORT,EC20_WAKEUP_PIN,GPIO_PIN_SET);
}

/**
* @brief  EC20ģ�����������ʾ���ų�ʼ��PB14
* @param  none
* @retval none
*/
#if 1==EC20_USE_RI
static void EC20_RI_PIN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_RI_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_RI_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_RI_PORT,&GPIO_InitStruct);	
}
#endif

/**
* @brief  EC20ģ��DTR���ų�ʼ��PB15
* @param  none
* @retval none
*/
#if 1==EC20_USE_DTR
static void EC20_DTR_PIN_init(void)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_DTR_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_DTR_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_DTR_PORT,&GPIO_InitStruct);
}
#endif

/**
* @brief  EC20ģ�����ģʽ�������ų�ʼ��PA6
* @param  none
* @retval none
*/
#if 1==EC20_USE_AIRPLANE
static void EC20_airplane_PIN_init(void)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_AIRPLANE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_AIRPLANE_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_AIRPLANE_PORT,&GPIO_InitStruct);
}
#endif

#if 1==EC20_USE_READY
/**
* @brief  EC20ģ��˯��״̬���ų�ʼ��PB0
* @param  none
* @retval none
*/
static void bsp_EC20_ready_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	EC20_READY_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=EC20_READY_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(EC20_READY_PORT,&GPIO_InitStruct);	
}
#endif
/**
* @brief  EC20ģ��弶Ӳ����ʼ��
* @param  none
* @retval none
*/
void bsp_EC20_Hardware_init(void)
{
	  /*USART2��ʼ��*/
	  bsp_USART2_init();

  	/*EC20�ܵ�Դʹ�ܳ�ʼ��*/
	  EC20_Power_EN_PIN_init();

	  /*EC20ģ��˯�߿������ų�ʼ��*/
	  EC20_WKP_PIN_init();/*low*/

	  /*EC20��λ���ų�ʼ��*/
	  EC20_Reset_PIN_init();/*high*/

	  /*EC20 PWRKEY���ų�ʼ��*/
	  EC20_Power_PIN_init();	
}
