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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
#define MAX_RECV_LEN 128

char  recv[100];
int recvtype=0;

int ONOFF_FLG=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);

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
//setlights
void setLights(){
	if(ONOFF_FLG==0){
		HAL_GPIO_WritePin(ONOFF_1_GPIO_Port,ONOFF_1_Pin,GPIO_PIN_SET);
	}
	if(ONOFF_FLG==1){
		HAL_GPIO_WritePin(ONOFF_1_GPIO_Port,ONOFF_1_Pin,GPIO_PIN_RESET);
	}
}
//processData
void processData(uint8_t* rx_buff){
	if(strcmp(rx_buff,"ON\n")==0){
		ONOFF_FLG=1;
		printf("l is on\n");
	}
	if(strcmp(rx_buff,"OFF\n")==0){
		ONOFF_FLG=0;
		printf("l is off\n");
	}
	setLights();
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("Wireless Control Ceiling Lights\n");         
	rxit_ok = HAL_UART_Receive_IT(&huart1, pBuf, 1); 
	rxit_ok2 = HAL_UART_Receive_IT(&huart2, pBuf2, 1);  
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ONOFF_1_Pin|ONOFF_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BLUE_Pin|GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ONOFF_1_Pin ONOFF_2_Pin */
  GPIO_InitStruct.Pin = ONOFF_1_Pin|ONOFF_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BLUE_Pin GREEN_Pin */
  GPIO_InitStruct.Pin = BLUE_Pin|GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
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
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
