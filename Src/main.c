/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "colorset.h"
#include "ws2812b.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define MAX_RECV_LEN 128

char  recv[100];
int recvtype=0;

int ONOFF_FLG=0;

char g[] ="255";
char b[] ="255";
char r[] ="255";


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t rx1_buff[MAX_RECV_LEN] = {0};  // 串口接收数据缓冲
uint8_t * pBuf = rx1_buff;  // 当前接收字节存放位置指针
uint8_t line_flag = 0;      // 一行数据接收标志
HAL_StatusTypeDef rxit_ok; 	// 接收中断是否开启
uint8_t rx2_buff[MAX_RECV_LEN] = {0};  // 串口接收数据缓冲
uint8_t * pBuf2 = rx2_buff; // 当前接收字节存放位置指针
uint8_t line_flag2 = 0;     // 一行数据接收标志
HAL_StatusTypeDef rxit_ok2; // 接收中断是否开启
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//Transmit data to uart
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff); // 向串口1发送一个字符
		HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff); // 向串口1发送一个字符
    return 0;
}
//spilit
void split( char **arr, char *str, const char *del)//字符分割函数的简单定义和实知现
{
	char *s =NULL;
	s=strtok(str,del);
	while(s != NULL)
	{
	*arr++ = s;
	s = strtok(NULL,del);
	}
}
//setlights
void setLights(){
	if(ONOFF_FLG==1){
		char *rx_t=rx2_buff;
		char *token;
		token=strtok(rx_t,"|");
		strcpy(g,token);

		token=strtok(NULL,"|");
		strcpy(b,token);

		token=strtok(NULL,"|");
		strcpy(r,token);
		
		uint8_t g_t=atoi(g);
		uint8_t b_t=atoi(b);
		uint8_t r_t=atoi(r);
		setAllPixelColor(g_t,b_t,r_t);
    WS2812B_Show();
	
}
	}
//processData
void processData(uint8_t* rx_buff){
	if(strcmp(rx_buff,"ON\n")==0){
		ONOFF_FLG=1;
	}else
	if(strcmp(rx_buff,"OFF\n")==0){
		ONOFF_FLG=0;
		setAllPixelColor(0,0,0);
    WS2812B_Show();
	}else
	{
		setLights();
	}

}
//process BlueTooth data
void ifRecData(){
		if (line_flag) {    // 如果串口1接收到一行数据
			printf("rx1_rec_data = %s", rx1_buff);        // 打印输出接收内容
			memset(rx1_buff, 0, sizeof(rx1_buff));   // 清空串口1缓存区
			pBuf = rx1_buff;// 重新将串口1接收数据的存放指针指向接收缓存的头部
			(&huart1)->pRxBuffPtr = pBuf;    // 重新将串口1结构体中的接收缓冲指针指向缓冲数组头部
			line_flag = 0;  // 串口1接收标志清零
		}
		if (line_flag2) {		// 如果串口2接收到一行数据
			processData(rx2_buff);
			memset(rx2_buff, 0, sizeof(rx2_buff));   // 清空串口2缓存区
			pBuf2 = rx2_buff;// 重新将串口2接收数据的存放指针指向接收缓存的头部
			(&huart2)->pRxBuffPtr = pBuf2;    // 重新将串口2结构体中的接收缓冲指针指向缓冲数组头部
			line_flag2 = 0;  // 串口2接收标志清零
		}
		if (rxit_ok != HAL_OK)		// 如果串口1接收中断还没有启动，尝试再次启动
			rxit_ok = HAL_UART_Receive_IT(&huart1, pBuf, 1);
		if (rxit_ok2 != HAL_OK)		// 如果串口2接收中断还没有启动，尝试再次启动
			rxit_ok2 = HAL_UART_Receive_IT(&huart2, pBuf, 1);
}
//uart interupt callback funtion
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
  uint8_t cnt = 0; // 临时变量，用于重复计数
  if (UartHandle->Instance == USART1)
  {
    ++ pBuf;  // 已接收一个字节数据，当前存储位置指针后移
    if(pBuf == rx1_buff + MAX_RECV_LEN)  // 如果指针已移出数组边界
        pBuf = rx1_buff;    // 重新指向数组开头
    else if(*(pBuf - 1) == '\n')  // 如果之前接收到‘\n’换行符，则表示接收完成
        line_flag  = 1;
    // 重新开启接收中断
    do {
        rxit_ok = HAL_UART_Receive_IT(UartHandle, pBuf, 1);
        if (++cnt >= 5)
            break;
    } while(rxit_ok != HAL_OK);
  }
  else if(UartHandle->Instance == USART2)
  {
    ++ pBuf2;  // 已接收一个字节数据，当前存储位置指针后移
    if(pBuf2 == rx2_buff + MAX_RECV_LEN)  // 如果指针已移出数组边界
        pBuf2 = rx2_buff;    // 重新指向数组开头
    else if(*(pBuf2 - 1) == '\n')  // 如果之前接收到‘\n’换行符，则表示接收完成
        line_flag2  = 1;
    // 重新开启接收中断
    do {
        rxit_ok2 = HAL_UART_Receive_IT(UartHandle, pBuf2, 1);
        if (++cnt >= 5)
            break;
    } while(rxit_ok2 != HAL_OK);
  }
	ifRecData();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("Wireless Control Ceiling Lights\n");         
	rxit_ok = HAL_UART_Receive_IT(&huart1, pBuf, 1); 
	rxit_ok2 = HAL_UART_Receive_IT(&huart2, pBuf2, 1);  

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  WS2812B_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 40-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
