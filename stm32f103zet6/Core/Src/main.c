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
int temperature = 0;//��һ����ʼ���������ж�
int humidity = 0;
int light_intensity = 0;
int smoke = 0;

//���ڽ��ջ��棨1�ֽڣ�
uint8_t buf=0;
//�����������ֽ��� 200���ɸ����������
#define UART2_REC_LEN 200

// ���ջ���, ���ڽ��յ������ݷ��������������UART2_REC_LEN���ֽ�
uint8_t UART2_RX_Buffer[UART2_REC_LEN];

// ����״̬
// bit15�� ������ɱ�־
// bit14�� ���յ�0x0d
// bit13~0�� ���յ�����Ч�ֽ���Ŀ
uint16_t UART2_RX_STA=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// ������ɻص��������յ�һ�����ݺ������ﴦ��
// ͨ���жϵķ������ܴ��ڹ��߷��͵��ַ���
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// �ж��ж������ĸ����ڴ�����
	if(huart->Instance == USART2)
	{
		// �жϽ����Ƿ���ɣ�UART2_RX_STA bit15 λ�Ƿ�Ϊ1��
		if((UART2_RX_STA & 0x8000) == 0)
		{
			// ����Ѿ��յ��� 0x0d ���س�����
			if(UART2_RX_STA & 0x4000)
			{
				// ������ж��Ƿ��յ� 0x0a �����У�
				if(buf == 0x0a)
					// ��� 0x0a �� 0x0d ���յ����� bit15 λ��Ϊ1
					UART2_RX_STA |= 0x8000;
				else
					// ������Ϊ���մ������¿�ʼ
					UART2_RX_STA = 0;
			}
			else // ���û���յ��� 0x0d ���س���
			{
				//�����ж��յ�������ַ��Ƿ��� 0x0d ���س���
				if(buf == 0x0d)
				{
					// �ǵĻ��� bit14 λ��Ϊ1
					UART2_RX_STA |= 0x4000;
				}
				else
				{
					// ���򽫽��յ������ݱ����ڻ���������
					UART2_RX_Buffer[UART2_RX_STA & 0X3FFF] = buf;
					UART2_RX_STA++;
					 // ����������ݴ���UART2_REC_LEN��200�ֽڣ��������¿�ʼ����
					if(UART2_RX_STA > UART2_REC_LEN - 1)
						UART2_RX_STA = 0;
				}
			}
		}
		// ���¿����ж�
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

  //oled��Ļ��ʼ��
  OLED_Init();
  OLED_Clear();//����
  OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
  //unsigned char i;
  HAL_UART_Receive_IT(&huart2, &buf, 1);//�������ڽ����ж�
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //�ж��жϴ����Ƿ�������
	  if(UART2_RX_STA & 0x8000)
	  {

		  //������ɺ󣬶��ַ������в���
		  if (sscanf((char *)UART2_RX_Buffer, "temperature:%2d, humidity:%2d, Light intensity:%3d, smoke:%2d\r\n", &temperature, &humidity, &light_intensity, &smoke) == 4)
		  {
			  sprintf(data, "temperature:%d��C, humidity:%d%%, Light intensity:%dLux, smoke:%dppm\r\n",temperature, humidity, light_intensity, smoke);
			  printf(data);

			  //��Json���ݷ���esp32���ϴ���������
			  sprintf(JsonData, "{\"temperature\":%d, \"humidity\":%d, \"LightLux\":%d, \"smoke\":%d}",temperature, humidity, light_intensity, smoke);
			  HAL_UART_Transmit(&huart3, (uint8_t *)&JsonData, strlen(JsonData), 100);

			  //������ֵ�������¶�>=30, ʪ��>=80, ����>=30
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

		  // ���յ������ݷ��͵�����
		  //HAL_UART_Transmit(&huart1, UART2_RX_Buffer, UART2_RX_STA & 0x3fff, 0xffff);
		  // �ȴ��������
		  while(huart2.gState != HAL_UART_STATE_READY);
		  printf("\r\n");
		  // ���¿�ʼ��һ�ν���
		  UART2_RX_STA = 0;
	  }

	  //��Ļ��ʾ����
	  //�¶�
	  OLED_ShowString(0, 0, (uint8_t *)&"Temp :", 16, 1);
	  OLED_ShowNum(64, 0, temperature, 2, 16, 1);
	  OLED_ShowChar(80, 0, (uint8_t)'^', 16, 1);
	  OLED_ShowChar(88, 0, (uint8_t)'C', 16, 1);

	  //ʪ��
	  OLED_ShowString(0, 16, (uint8_t *)&"Hum :", 16, 1);
	  OLED_ShowNum(64, 16, humidity, 2, 16, 1);
	  OLED_ShowChar(80, 16, (uint8_t)'%', 16, 1);

	  //����ǿ��
	  OLED_ShowString(0, 32, (uint8_t *)&"Light :", 16, 1);
	  OLED_ShowNum(64, 32, light_intensity, 3, 16, 1);
	  OLED_ShowString(88, 32, (uint8_t *)&"Lux", 16, 1);

	  //����Ũ��
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
