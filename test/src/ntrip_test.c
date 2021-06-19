#include "ntrip_test.h"

/*配置ntrip参数格式：
  NTRIP,ip,port,挂载点,用户名,密码
  example:ip为120.27.92.24，port为2235,挂载点为RTCM32_GGB,用户名为qxrfa003,密码为tings11
	 则配置参数为：NTRIP,120.27.92.24,2235,RTCM32_GGB,qxrfa003,tings11
  注意：必须使用英文状态下的逗号','，密码结束要加换行回车
*/

int ntrip_port = 0;
char ntrip_ip[32] = {0};
char ntrip_point[32] = {0};
char ntrip_username[32] = {0};
char ntrip_password[32] = {0};


uint8_t ntrip_buf[RTCM_SERVER_CONFIG_SIZE]={0};

void ntrip_ip_port_get(uint8_t *pbuf,uint8_t len)
{
  uint16_t index_A = 0, index_B = 0;
  char buf[64]={0};

  if (NULL != strstr((char*)pbuf, "RTK,"))
	 {
		  /*get ip*/
		  index_A = Find_LineIndex(pbuf, 1, len);
		  index_B = Find_LineIndex(pbuf, 2, len);
		  memcpy(ntrip_ip, pbuf + index_A, index_B - index_A - 1);

		  /*get port*/
		  index_A = Find_LineIndex(pbuf, 3, len);
		  if (index_A - index_B)
		  {
		    sscanf((char*)&pbuf[index_B], "%d", &ntrip_port);
		  }
    sprintf(buf, "AT+QIOPEN=1,1,\"TCP\",\"%s\",%d,0,1\r\n", ntrip_ip, ntrip_port);

		 // NTRIP_LOG("%s", buf);
  }
  else
		{
    NTRIP_LOG("ERROR:has not RTK config\r\n");
		}
}

void ntrip_user_info_get(uint8_t *pbuf,uint8_t len)
{
  uint16_t index_A = 0, index_B = 0;
  uint8_t *pret=NULL;

  if (NULL != strstr((char*)pbuf, "RTK,"))
	 {
		  /*挂载点*/
		  index_A = Find_LineIndex(pbuf, 3, len);
		  index_B = Find_LineIndex(pbuf, 4, len);
		  memcpy(ntrip_point, pbuf + index_A, index_B - index_A - 1);
		  NTRIP_LOG("point：%s", ntrip_point);

		  /*用户名*/
	  	index_A = Find_LineIndex(pbuf, 5, len);
		  memcpy(ntrip_username, pbuf + index_B, index_A - index_B - 1);
		  NTRIP_LOG("username：%s", ntrip_username);

		  /*密码*/
    pret=(uint8_t*)strstr((char*)pbuf+index_A, "\r\n");
    if((NULL != pret)&&((pret-&pbuf[index_A])<32))
    {
      memcpy(ntrip_password, pbuf + index_A, pret-&pbuf[index_A]);
	  	  NTRIP_LOG("password：%s", ntrip_password);
    }
	  	
  }
		else
		{
    NTRIP_LOG("has not parameter in eeprom\r\n");
		}
}

void ntrip_para_load(void)
{
  EEPROM_ntrip_para_read(ntrip_buf);
  ntrip_ip_port_get(ntrip_buf,RTCM_SERVER_CONFIG_SIZE);
  ntrip_user_info_get(ntrip_buf,RTCM_SERVER_CONFIG_SIZE);
}

