/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "stm32f4_discovery_accelerometer.h"
#include "usbd_cdc.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t receive_data[64];
uint8_t receive_serial[64];
uint8_t myData[64] = "hello\n";
int16_t XYZ[3];
uint16_t adcValues[3];
uint8_t inputPort[8];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef SEMIHOSTING
extern int initialise_monitor_handles(void); //semihosting
#endif
void parseRx(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	uint16_t count=0;
	uint16_t checksum=0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USB_DEVICE_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
#ifdef SEMIHOSTING
  initialise_monitor_handles(); //semihosting
#endif
  BSP_ACCELERO_Init();

  HAL_ADC_Start_DMA(&hadc1, adcValues, sizeof(uint16_t)*3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  checksum=0;

	  //accelerometer
	  BSP_ACCELERO_GetXYZ(XYZ);

	  sprintf(myData, "###%03d%04X%04X%04X", (uint16_t)count,(uint16_t)XYZ[0], (uint16_t)XYZ[1], (uint16_t)XYZ[2]);
#ifdef SEMIHOSTING
	  printf("X: %0.2f\tY: %0.2f\tZ: %0.2f\n", (float)XYZ[0], (float)XYZ[1], (float)XYZ[2]);
#endif
	  //3 ADCs
	  sprintf(myData + 18, "%04d%04d%04d", (uint16_t)adcValues[0],(uint16_t)adcValues[1],(uint16_t)adcValues[2]);

	  //8 Digital inputs
	  inputPort[7] = (uint8_t)HAL_GPIO_ReadPin(IN7_GPIO_Port,IN7_Pin);
	  inputPort[6] = (uint8_t)HAL_GPIO_ReadPin(IN6_GPIO_Port,IN6_Pin);
	  inputPort[5] = (uint8_t)HAL_GPIO_ReadPin(IN5_GPIO_Port,IN5_Pin);
	  inputPort[4] = (uint8_t)HAL_GPIO_ReadPin(IN4_GPIO_Port,IN4_Pin);
	  inputPort[3] = (uint8_t)HAL_GPIO_ReadPin(IN3_GPIO_Port,IN3_Pin);
	  inputPort[2] = (uint8_t)HAL_GPIO_ReadPin(IN2_GPIO_Port,IN2_Pin);
	  inputPort[1] = (uint8_t)HAL_GPIO_ReadPin(IN1_GPIO_Port,IN1_Pin);
	  inputPort[0] = (uint8_t)HAL_GPIO_ReadPin(IN0_GPIO_Port,IN0_Pin);

	  //8 digital inputs
	  sprintf(myData + 30, "%0d", inputPort[7]); //strlen(myData)
	  sprintf(myData + 31, "%0d", inputPort[6]);
	  sprintf(myData + 32, "%0d", inputPort[5]);
	  sprintf(myData + 33, "%0d", inputPort[4]);
	  sprintf(myData + 34, "%0d", inputPort[3]);
	  sprintf(myData + 35, "%0d", inputPort[2]);
	  sprintf(myData + 36, "%0d", inputPort[1]);
	  sprintf(myData + 37, "%0d", inputPort[0]);

	  //Blue Button
	  sprintf(myData + 38, "%0d", HAL_GPIO_ReadPin(blueButton_GPIO_Port, blueButton_Pin));

	  //calculate checksum
	  for(int loop=3; loop<=strlen(myData); loop++)
	  {
		  checksum += myData[loop];
	  }
	  checksum %=1000;	//3 digits
	  sprintf(myData + 39, "%03d\r\n", checksum);

	  //USBD_CDC_SetTxBuffer(&hUsbDeviceFS, myData, 7);
	  CDC_Transmit_FS(myData, 44);  //micro usb 		-- strlen((const char*)myData)

	  HAL_UART_Transmit(&huart2, myData, 44, 1000); //mini usb uart to stlink 	-- strlen((const char*)myData)
	  //printf("%s", myData);  //semihosting - console
	  count++;
	  count%=1000;	//000-999

	  parseRx();


	  HAL_Delay(10);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT0_Pin|OUT1_Pin|OUT2_Pin|OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, OUT4_Pin|OUT5_Pin|OUT6_Pin|OUT7_Pin 
                          |greenLED_Pin|orangeLED_Pin|redLED_Pin|blueLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : IN3_Pin IN1_Pin IN2_Pin */
  GPIO_InitStruct.Pin = IN3_Pin|IN1_Pin|IN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : IN0_Pin */
  GPIO_InitStruct.Pin = IN0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IN0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : blueButton_Pin */
  GPIO_InitStruct.Pin = blueButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(blueButton_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT0_Pin OUT1_Pin OUT2_Pin OUT3_Pin */
  GPIO_InitStruct.Pin = OUT0_Pin|OUT1_Pin|OUT2_Pin|OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT4_Pin OUT5_Pin OUT6_Pin OUT7_Pin 
                           greenLED_Pin orangeLED_Pin redLED_Pin blueLED_Pin */
  GPIO_InitStruct.Pin = OUT4_Pin|OUT5_Pin|OUT6_Pin|OUT7_Pin 
                          |greenLED_Pin|orangeLED_Pin|redLED_Pin|blueLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : IN7_Pin */
  GPIO_InitStruct.Pin = IN7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IN7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN5_Pin IN6_Pin IN4_Pin */
  GPIO_InitStruct.Pin = IN5_Pin|IN6_Pin|IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void parseRx(void)
{
	  HAL_UART_Receive(&huart2, receive_serial, 50, 500 );
	  if(strlen(receive_serial)>0)
	  {
		  int state=0; //state machine
		  volatile int rxChecksum=0;
		  int calcChecksum=0;

		  HAL_UART_Transmit(&huart2, receive_serial, strlen((const char*)receive_serial), 500);
		  for(int loop=0; loop<strlen((const char*)receive_serial); loop++)
		  {
			  switch(state)
			  {
			  case 0:
			  case 1:
			  case 2:
				  if(receive_serial[loop]=='#')
				  {
					  state++;
				  }
				  else
				  {
					  state=0;
				  }
				  break;
				  //digital outputs
			  case 3:
				  //checksum 576-588 with onboard LEDs, was 384-392
				  for(int count=3; count<15; count++)
				  {
					  calcChecksum+=receive_serial[count];
				  }
				  rxChecksum = (receive_serial[15]-'0') * 100;
				  rxChecksum += (receive_serial[16]-'0') * 10;
				  rxChecksum += (receive_serial[17]-'0');

				  if(calcChecksum==rxChecksum) //valid checksum?
				  {
				  HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin,receive_serial[loop]-'0'); 	//yes
				  state++;
				  }
				  else
				  {
					  state=0;	//no
				  }
				  break;
			  case 4:
				  HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 5:
				  HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 6:
				  HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 7:
				  HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 8:
				  HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 9:
				  HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 10:
				  HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 11:
				  HAL_GPIO_WritePin(blueLED_GPIO_Port, blueLED_Pin, receive_serial[loop]-'0');
				  state++;
				  break;
			  case 12:
				  HAL_GPIO_WritePin(redLED_GPIO_Port, redLED_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 13:
				  HAL_GPIO_WritePin(orangeLED_GPIO_Port, orangeLED_Pin,receive_serial[loop]-'0');
				  state++;
				  break;
			  case 14:
				  HAL_GPIO_WritePin(greenLED_GPIO_Port, greenLED_Pin,receive_serial[loop]-'0');
				  state++;
				  break;


			  }



			  }

		  }

		  receive_serial[0]=0;


}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
