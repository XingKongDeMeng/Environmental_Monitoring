/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "oled.h"
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

/* USER CODE BEGIN PV */
int temperature = 0;//给一个初始化，用于判断
int humidity = 0;
int light_intensity = 0;
int smoke = 0;

//串口接收缓存（1字节）
uint8_t buf=0;
//定义最大接收字节数 200，可根据需求调整
#define UART2_REC_LEN 200

// 接收缓冲, 串口接收到的数据放在这个数组里，最大UART2_REC_LEN个字节
uint8_t UART2_RX_Buffer[UART2_REC_LEN];

// 接收状态
// bit15， 接收完成标志
// bit14， 接收到0x0d
// bit13~0， 接收到的有效字节数目
uint16_t UART2_RX_STA=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// 接收完成回调函数，收到一个数据后，在这里处理
// 通过中断的方法接受串口工具发送的字符串
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// 判断中断是由哪个串口触发的
	if(huart->Instance == USART2)
	{
		// 判断接收是否完成（UART2_RX_STA bit15 位是否为1）
		if((UART2_RX_STA & 0x8000) == 0)
		{
			// 如果已经收到了 0x0d （回车），
			if(UART2_RX_STA & 0x4000)
			{
				// 则接着判断是否收到 0x0a （换行）
				if(buf == 0x0a)
					// 如果 0x0a 和 0x0d 都收到，则将 bit15 位置为1
					UART2_RX_STA |= 0x8000;
				else
					// 否则认为接收错误，重新开始
					UART2_RX_STA = 0;
			}
			else // 如果没有收到了 0x0d （回车）
			{
				//则先判断收到的这个字符是否是 0x0d （回车）
				if(buf == 0x0d)
				{
					// 是的话则将 bit14 位置为1
					UART2_RX_STA |= 0x4000;
				}
				else
				{
					// 否则将接收到的数据保存在缓存数组里
					UART2_RX_Buffer[UART2_RX_STA & 0X3FFF] = buf;
					UART2_RX_STA++;
					 // 如果接收数据大于UART2_REC_LEN（200字节），则重新开始接收
					if(UART2_RX_STA > UART2_REC_LEN - 1)
						UART2_RX_STA = 0;
				}
			}
		}
		// 重新开启中断
		HAL_UART_Receive_IT(&huart2, &buf, 1);
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
  char data[100];
  char JsonData[100];

  //uint8_t code[100] = {"Hello OLED lt"};
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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  //oled屏幕初始化
  OLED_Init();
  OLED_Clear();//清屏
  OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
  //unsigned char i;
  HAL_UART_Receive_IT(&huart2, &buf, 1);//开启串口接收中断
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //判断判断串口是否接收完成
	  if(UART2_RX_STA & 0x8000)
	  {

		  //接收完成后，对字符串进行操作
		  if (sscanf((char *)UART2_RX_Buffer, "temperature:%2d, humidity:%2d, Light intensity:%3d, smoke:%2d\r\n", &temperature, &humidity, &light_intensity, &smoke) == 4)
		  {
			  sprintf(data, "temperature:%d°C, humidity:%d%%, Light intensity:%dLux, smoke:%dppm\r\n",temperature, humidity, light_intensity, smoke);
			  printf(data);

			  //将Json数据发给esp32并上传到阿里云
			  sprintf(JsonData, "{\"temperature\":%d, \"humidity\":%d, \"LightLux\":%d, \"smoke\":%d}",temperature, humidity, light_intensity, smoke);
			  HAL_UART_Transmit(&huart3, (uint8_t *)&JsonData, strlen(JsonData), 100);

			  //超过阈值报警，温度>=30, 湿度>=80, 烟雾>=30
			  if(temperature >= 30)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, SET);
			  }

			  else if(humidity >= 80)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, SET);
			  }

			  else if(smoke >= 80)
			  {
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, SET);
			  }

			  else
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, RESET);

		  }

		  // 将收到的数据发送到串口
		  //HAL_UART_Transmit(&huart1, UART2_RX_Buffer, UART2_RX_STA & 0x3fff, 0xffff);
		  // 等待发送完成
		  while(huart2.gState != HAL_UART_STATE_READY);
		  printf("\r\n");
		  // 重新开始下一次接收
		  UART2_RX_STA = 0;
	  }

	  //屏幕显示数据
	  //温度
	  OLED_ShowString(0, 0, (uint8_t *)&"Temp :", 16, 1);
	  OLED_ShowNum(64, 0, temperature, 2, 16, 1);
	  OLED_ShowChar(80, 0, (uint8_t)'^', 16, 1);
	  OLED_ShowChar(88, 0, (uint8_t)'C', 16, 1);

	  //湿度
	  OLED_ShowString(0, 16, (uint8_t *)&"Hum :", 16, 1);
	  OLED_ShowNum(64, 16, humidity, 2, 16, 1);
	  OLED_ShowChar(80, 16, (uint8_t)'%', 16, 1);

	  //光照强度
	  OLED_ShowString(0, 32, (uint8_t *)&"Light :", 16, 1);
	  OLED_ShowNum(64, 32, light_intensity, 3, 16, 1);
	  OLED_ShowString(88, 32, (uint8_t *)&"Lux", 16, 1);

	  //烟雾浓度
	  OLED_ShowString(0, 48, (uint8_t *)&"Smoke :", 16, 1);
	  OLED_ShowNum(64, 48, smoke, 2, 16, 1);
	  OLED_ShowChar(80, 48, (uint8_t)'%', 16, 1);

	  OLED_Refresh();
	  HAL_Delay(100);


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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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

  /** Initializes the CPU, AHB and APB buses clocks
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
