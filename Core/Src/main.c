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
#include "cmsis_os.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmp180_for_stm32_hal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "onewire.h"
#include "ds18b20.h"
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
osThreadId Bmp180Handle;
osThreadId Ds18b20Handle;
osThreadId BlinkyHandle;


//osMutexDef (MutexIsr);                                     // Mutex name definition
//osMutexId mutex_id;                                        // Mutex id



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void Bmp180_Init(osPriority Priority);
void Ds18b20_Init(osPriority Priority);

void Task_Bmp180(void const *argument);
void Task_Blinky(void const *argument);
void Task_Ds18b20(void const *argument);

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
	return ch;
}
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

	DS18B20_Config(DS18B20_Resolution_12bits);// olyan mintha a mutex el??tt kellene lennie...

//	DS18B20_Config(DS18B20_Resolution_12bits);//ennek itt kell lennie, mert k??l??nben nem m??k??dik :D
	//ha be akarom tenni pl. a Ds18b20_Init()-be akkor m??r nem j??... lehal a led taszk is...
	printf("\r\n************** APPLICATION *************\r\n");
	printf("****************************************\r\n");

	printf("Next version\r\n");

	//mutex_id = osMutexCreate  (osMutex (MutexIsr));
	// sz??ks??ge van a mutexre is... ??n ezt komolyan nem ??rtem...
	// nincs am??gy error handlerbe ilyenkor...

	Bmp180_Init(osPriorityNormal);

	//valami??rt m??g nem megy... :(
	// pedig mutexet is pr??b??ltam
	// n??ha az els?? ki??r??s ut??n lefagy, de beakasztja a ledvillogtat?? taszkot is...

	//asszem megvan :D
	// csak t??bb mne??ria kellett neki (256)
	Ds18b20_Init(osPriorityNormal);

	osPriority Task_BlinkyPriority = osPriorityNormal;
	osThreadDef(myTask_Blinky, Task_Blinky, Task_BlinkyPriority, 0, 128);
	BlinkyHandle = osThreadCreate(osThread(myTask_Blinky), NULL);


	///!!! BIZONY, BIZONY A TASZKOK MEM??RI??J??T KELLETT ??LL??TGATNI, HOGY A MUTEX N??LK??L MENJEN....
	//H??T EZ SEM MINDIG MEGY...
	//LEHET HOGY A DS18B20 NEM AZ IGAZI, LEHET N??HA RESETELNI K??NE, MERT N??HA 4095-??T ??R H??M??RS??KLETRE...
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

void Ds18b20_Init(osPriority Priority) {
	osThreadDef(myTask_Ds18b20, Task_Ds18b20, Priority, 0, 512);
	Ds18b20Handle = osThreadCreate(osThread(myTask_Ds18b20), NULL);

}

void Task_Ds18b20(void const *argument) {

	float temperature;
	char message[64];

	for (;;) {
		//osMutexWait    (mutex_id, 0);
		DS18B20_ReadAll();
		DS18B20_StartAll();
		uint8_t ROM_tmp[8];
		uint8_t i;
		for (i = 0; i < DS18B20_Quantity(); i++) {
			if (DS18B20_GetTemperature(i, &temperature)) {
				DS18B20_GetROM(i, ROM_tmp);
				memset(message, 0, sizeof(message));
				sprintf(message, "%d. ROM: %X%X%X%X%X%X%X%X Temp: %f\r\n", i,
						ROM_tmp[0], ROM_tmp[1], ROM_tmp[2], ROM_tmp[3],
						ROM_tmp[4], ROM_tmp[5], ROM_tmp[6], ROM_tmp[7],
						temperature);
				HAL_UART_Transmit(&huart2, message, strlen(message), 1000);
				printf("****************************************\r\n");
			}
		}
		//osMutexRelease(mutex_id);
		osDelay(2000);
	}
}

void Bmp180_Init(osPriority Priority) {
	osThreadDef(myTask_Bmp180, Task_Bmp180, Priority, 0, 512);
	Bmp180Handle = osThreadCreate(osThread(myTask_Bmp180), NULL);

	//TODO: a hibakezelel??st itt m??g meg k??ne oldani, mert ha nincs csatlakoztatva szenzor akkor beragad
#if 1
	/* Initializes BMP180 sensor and oversampling settings. */
	BMP180_SetI2C(&hi2c1);
	BMP180_SetOversampling(BMP180_ULTRA);
	/* Update calibration data. Must be called once before entering main loop. */
	BMP180_UpdateCalibrationData();
#endif
}

void Task_Bmp180(void const *argument) {
	for (;;) {
#if 1
		/* Reads temperature. */
		int32_t temperature = BMP180_GetRawTemperature();
		/* If you want a floating point number instead, you can call: */
		//float temperature = BMP180_GetTemperature();
		/* Reads pressure. */
		int32_t pressure = BMP180_GetPressure();
		char buffer[100];
		sprintf(buffer, "Temperature: %d.%d deg C\nPressure: %d Pa\n",
				(int) temperature / 10, (int) temperature % 10, (int) pressure);
		HAL_UART_Transmit(&huart2, buffer, strlen(buffer), 1000);
#endif
		printf("****************************************\r\n");
		osDelay(2000);
	}
}

void Task_Blinky(void const *argument) {
	for (;;) {
		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
		osDelay(500);
	}

}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
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
	__disable_irq();
	while (1) {
		printf("Error_Handler()\r\n");
		HAL_Delay(500);
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
