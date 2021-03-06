/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esp32.h"
#include "bt.h"
#include "adxl345.h"
#include "oled12864.h"
#include "wifi.h"
#include "filter.h"
#include "step.h"
#include "max30102.h"
#include "heartrate.h"

#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TOTAL_ACC 244
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t i = 0;
static uint16_t d = 0;
static uint32_t stepNum = 0;
static float32_t f = 0;
static float32_t hw = 0;
static WifiDataPack_3AxisAccWithTotal acc;
static uint16_t heartRate = 0;
static BtDataPack_3AxisAccWithTotal pdata_3Acc;

static uint16_t data_hr[] = { 88, 92, 100, 92, 98, 92, 92, 93, 92, 96, 92, 96, 93, 96, 86, 96, 90, 92, 96, 92, 93, 88, 90, 85, 92, 88, 96, 93, 96, 89, 89, 95, 90, 92, 85, 88, 88, 88, 92, 166, 98, 92, 86, 85, 90, 90, 90, 90, 86, 92, 96, 85, 85, 92, 88, 92, 93, 90, 93, 92, 92, 93, 93, 93, 90, 86, 90, 90, 181, 86, 90, 92, 92, 93, 96, 96, 92, 88, 90, 98, 93, 96, 96, 98, 90, 93, 46, 93, 92, 92, 93, 93, 88, 88, 88, 92, 90, 96, 90, 92, 95, 88, 88, 30, 93, 93, 92, 92, 85, 88, 88, 92, 93, 93, 92, 93, 88, 93, 93, 93, 93, 96, 93, 93, 93, 93, 86, 93, 88, 93, 89, 88, 90, 90, 92, 93, 46, 93, 92, 92, 93, 93, 88, 88, 88, 92, 90, 96, 90, 92, 95 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_SPI1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OledDisplayLine((uint8_t*)"oled init success");
	ADXL345_Init();
	OledDisplayLine((uint8_t*)"adxl345 init success");

	esp32WirelessUse = NO_WIRELESS;
	OLED_ShowString(0, 3, (uint8_t*)"no wireless mode", OLED_CHAR_SIZE8);
	for (i = 0; i < 50; i++)//select wireless mode and wating esp32 init
	{
		if (HAL_GPIO_ReadPin(wirelessSelect_GPIO_Port, wirelessSelect_Pin) == GPIO_PIN_RESET)
		{
			HAL_Delay(10);
			if (HAL_GPIO_ReadPin(wirelessSelect_GPIO_Port, wirelessSelect_Pin) == GPIO_PIN_RESET)
			{
				if (esp32WirelessUse == NO_WIRELESS)
				{
					esp32WirelessUse = USE_BLUETOOTH;
					OLED_ShowString(0, 3, (uint8_t*)"bluetooth mode  ", OLED_CHAR_SIZE8);
				}
				else if (esp32WirelessUse == USE_BLUETOOTH)
				{
					esp32WirelessUse = USE_WIFI;
					OLED_ShowString(0, 3, (uint8_t*)"wifi mode       ", OLED_CHAR_SIZE8);
				}
				else if (esp32WirelessUse == USE_WIFI)
				{
					esp32WirelessUse = NO_WIRELESS;
					OLED_ShowString(0, 3, (uint8_t*)"no wireless mode", OLED_CHAR_SIZE8);
				}
				HAL_Delay(10);
				while((HAL_GPIO_ReadPin(wirelessSelect_GPIO_Port, wirelessSelect_Pin) == GPIO_PIN_RESET))HAL_Delay(100);
			}
			
		}
		HAL_Delay(100);
	}
	i = 0;
	if (esp32WirelessUse == USE_BLUETOOTH)BtInit();
	else if (esp32WirelessUse == USE_WIFI)WifiInit();
	else OledDisplayLine((uint8_t*)"no wireless");

	ADXL345_RD_XYZ(&(acc.x), &(acc.y), &(acc.z));
	acc.f = (float)sqrt((double)acc.x*acc.x + (double)acc.y*acc.y + (double)acc.z*acc.z);
	FilterInit(acc.f);
	OLED_Clear();
	OledStepCountInit();
	OledHeartrateInit();

	HeartRateInit();
	HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		OledShowStepCountNum(stepNum);
//		if (i == 10)
//		{
//			OledDisplayHeartrateWave(d+30);
//			d++;
//			i = 0;
//			if (d > 130)d = 0;
//		}
//		i++;
		HAL_Delay(100);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle->Instance == USART1)Esp32RxCallback();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == htim3.Instance) {

		ADXL345_RD_XYZ(&(acc.x), &(acc.y), &(acc.z));
		acc.f = (float)sqrt((double)acc.x*acc.x + (double)acc.y*acc.y + (double)acc.z*acc.z);
		Filter(&(acc.f), &f, 1);
		stepNum += StepCount(&f);
		
		hw = GetHeartWaveWithFilter();
		heartRate = CalcHeartrate(&hw);
		if (heartRate)
		{
			OledShowHeartrateCountNum(heartRate);
			OledDisplayHeartrateWave(heartRate);
		}
		
		if (esp32WirelessUse == USE_BLUETOOTH)
		{
			pdata_3Acc.x = acc.x;
			pdata_3Acc.y = acc.y;
			pdata_3Acc.z = hw;
			pdata_3Acc.f = f;
			BtSendDatapack_3AxisAccWithTotal(&pdata_3Acc);
		}
		else if (esp32WirelessUse == USE_WIFI)
		{
			WifiSendDataPack_MixedAcc(&(f));
		}
	}
}
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

