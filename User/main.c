/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./bt05/bsp_bt05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>


unsigned int Task_Delay[NumOfTask]; 


BLTDev bltDevList;




char sendData[1024];
char linebuff[1024];



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	char* redata;
	uint16_t len;
	
	static uint8_t hc05_role=0;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	
	  //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	HC05_INFO("**********���߿���************");
	
	if(HC05_Init() == 0) {
		HC05_INFO("����ģ����������");
	}
	else
	{
		HC05_ERROR("����ģ���ⲻ����");
		while(1){
		}
	}


	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	

	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/	

	while(1)
	{
			//���Ӻ�ÿ��һ��ʱ������ջ�����
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;

				/*��ȡ����*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*��������Ƿ��и���*/
				if(linelen<200 && linelen != 0)
				{
					
					if(strcmp(redata,"ON\n")==0)
					{					
						HC05_INFO("LIGHT ON\r\n");
						//lights on

					}
					else if(strcmp(redata,"OFF\n")==0)
					{
						
						HC05_INFO("LIGHT OFF\r\n");
						//lights off

					}
					else
					{
						HC05_INFO("receive:\r\n%s",redata);
					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
		}
		
		//���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���

	}
}

/*********************************************END OF FILE**********************/
