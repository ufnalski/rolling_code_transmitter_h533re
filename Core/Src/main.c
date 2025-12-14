/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "fdcan.h"
#include "hash.h"
#include "i2c.h"
#include "icache.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRANSMITTER_SERIAL_NUMBER 0xDEADC0DE // 4-byte
#define SECRET_KEY 0xDEADBEEF  // 4-byte
#define ACTION_CODE 0xAC  // 1-byte
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;

/* USER CODE BEGIN PV */
uint32_t rolling_counter_tx = 0;
uint8_t button_pressed_flag = 0;

FDCAN_TxHeaderTypeDef txHeader;

union
{
	uint8_t Bytes[16];
	uint32_t Words[4];
} txData; // CAN FD payload: 4-byte serial number, 11-byte hash of secret key appended with a counter, 1-byte action code

union
{
	uint8_t Bytes[12];
	uint32_t Words[3];
} dataToBeHashed;

uint8_t SHA256Digest[32];

char lcd_line[32];
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
	MX_ICACHE_Init();
	MX_FDCAN1_Init();
	MX_I2C3_Init();
	MX_HASH_Init();
	/* USER CODE BEGIN 2 */

	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(20, 0);
	ssd1306_WriteString("ufnalski.edu.pl", Font_6x8, White);
	ssd1306_SetCursor(20, 12);
	ssd1306_WriteString("Rolling code TX", Font_6x8, White);
	ssd1306_UpdateScreen();

	dataToBeHashed.Words[0] = SECRET_KEY;

	if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}

	txHeader.IdType = FDCAN_STANDARD_ID;
	txHeader.TxFrameType = FDCAN_DATA_FRAME;
	txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txHeader.BitRateSwitch = FDCAN_BRS_OFF;
	txHeader.FDFormat = FDCAN_FD_CAN;
	txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	txHeader.MessageMarker = 0;

	/* USER CODE END 2 */

	/* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
	BspCOMInit.BaudRate = 115200;
	BspCOMInit.WordLength = COM_WORDLENGTH_8B;
	BspCOMInit.StopBits = COM_STOPBITS_1;
	BspCOMInit.Parity = COM_PARITY_NONE;
	BspCOMInit.HwFlowCtl = COM_HWCONTROL_NONE;
	if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
	{
		Error_Handler();
	}

	/* USER CODE BEGIN BSP */

	/* -- Sample board code to send message over COM1 port ---- */
	printf("\r\nWelcome to KeeLoq world of rolling/hopping codes!\r\n");

	/* USER CODE END BSP */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		if (button_pressed_flag == 1)
		{
			HAL_Delay(50);  // primitive button debounce
			button_pressed_flag = 0;

			if (HAL_GPIO_ReadPin(WRONG_KEY_FOB_GPIO_Port, WRONG_KEY_FOB_Pin)
					== GPIO_PIN_RESET)
			{
				txData.Words[0] = TRANSMITTER_SERIAL_NUMBER;
			}
			else
			{
				txData.Words[0] = TRANSMITTER_SERIAL_NUMBER + 1; // emulates wrong key fob
			}

			dataToBeHashed.Words[1] = rolling_counter_tx;
			HAL_HASH_Start(&hhash, dataToBeHashed.Bytes, 8, SHA256Digest, 100);

			for (uint8_t i = 0; i < 11; i++)
			{
				txData.Bytes[i + 4] = SHA256Digest[i];
			}

			txData.Bytes[15] = ACTION_CODE;

			txHeader.DataLength = FDCAN_DLC_BYTES_16;  // in FDCAN we can go beyond 8 bytes
			txHeader.Identifier = 0x101;

			if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData.Bytes)
					!= HAL_OK)
			{
				Error_Handler();
			}

			if (HAL_GPIO_ReadPin(WRONG_KEY_FOB_GPIO_Port, WRONG_KEY_FOB_Pin)
					== GPIO_PIN_RESET)
			{
				rolling_counter_tx++;
			}

			sprintf(lcd_line, "TX CNT: %lu", rolling_counter_tx);
			ssd1306_SetCursor(2, 24);
			ssd1306_WriteString(lcd_line, Font_6x8, White);

			sprintf(lcd_line, "TX: %08lX %08lX", txData.Words[0],
					txData.Words[1]);
			ssd1306_SetCursor(2, 36);
			ssd1306_WriteString(lcd_line, Font_6x8, White);

			sprintf(lcd_line, "   %08lX %02lX %06lX", txData.Words[2],
					(txData.Words[3] & 0xFF000000) >> 24,
					txData.Words[3] & 0x00FFFFFF);
			ssd1306_SetCursor(2, 48);
			ssd1306_WriteString(lcd_line, Font_6x8, White);

			ssd1306_UpdateScreen();

		}
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
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
	{
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
	RCC_OscInitStruct.CSIState = RCC_CSI_ON;
	RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the programming delay
	 */
	__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_1);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BLUE_BUTTON_Pin)
	{
		button_pressed_flag = 1;
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
#ifdef USE_FULL_ASSERT
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
