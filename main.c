/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t s=0; //Elegir numero de rondas
uint8_t rondas=0; //Numero de rondas a jugar
uint8_t ModoJ=0; //Seleccionar modo de juego 1 o 2 personas
uint8_t tipo=0; // Tipo de juego
uint8_t p=0; //print
uint8_t inicio=0; //iniciar juego

char msg[]="\r\n Bienvenido al juego de reccion, en este juego se pondran a prueba \r\n"
" tus reflejos con alertas visuales o sonoras \r\n";//mensaje de bienvenida
char PInicio[]="Teclee el numero corresponiende al modo de juego \r\n"
"Seleccione el modo de juego \r\n 1.-Un jugador \r\n 2.-Dos jugadores\r\n";
char MJ1[]="Entro en modo de un jugador:\r\n"
"A.- Sonido \r\n B.- Luz \r\n C.- Aleatorio \r\n D.- Ver mejor tiempo\r\n"
"E.- Ultimos 5 resultados \r\n 0.- Regresar";
char MJ2[]="Entro en el modo de dos jugadores:\r\n"
"A.- Sonido \r\n B.- Luz \r\n C.- Aleatorio \r\n 0.-Regresar\r\n";
char Rx_data[1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	extern char Rx_data[1];
	extern uint8_t s;
	extern uint8_t ModoJ;
	extern uint8_t p;
	extern uint8_t tipo;

	if(huart->Instance == USART2)
	{
		if(s==0)
		{
			if( (!strcmp(Rx_data,"\n"))||(!strcmp(Rx_data,"\r")))
			{//fin de mensaje
				p=1;
			}
			if(!strcmp(Rx_data,"0" ))
			{//Comprobar si es cero
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)PInicio,sizeof(PInicio)); //print pantalla de inicio
				ModoJ=0;
			}
			if(!strcmp(Rx_data,"1"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)MJ1,sizeof(MJ1));
				ModoJ=1;
			}
			if(!strcmp(Rx_data,"2"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)MJ2,sizeof(MJ2));
				ModoJ=2;
			}
			if(!strcmp(Rx_data,"A"))
			{
				tipo=1;
			}
			if(!strcmp(Rx_data,"B"))
			{
				tipo=2;
			}
			if(!strcmp(Rx_data,"C"))
			{
				tipo=3;
			}
			if(!strcmp(Rx_data,"D"))
			{
				tipo=4;
			}
			if(!strcmp(Rx_data,"E"))
			{
				tipo=5;
			}
		}
		if(s==1)
		{
			if( (!strcmp(Rx_data,"\n"))||(!strcmp(Rx_data,"\r")))
			{
				p=1;
			}
			else
			{
				s=strtol(Rx_data, NULL, 10);
			}

		}
		HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);
	}
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
	char msg1[]="Sonido\r\n";
	char msg2[]="Luz\r\n";
	char msg3[]="Aleatorio\r\n";
	char msg4[]="Mejor tiempo\r\n";
	char msg5[]="Ultimos 5 juegos\r\n";
	char MJ21[]="Digite el numero de rondas entre 1 y 9 \r\n";
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
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit(&huart2, (uint8_t *)msg,strlen(msg),HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2, (uint8_t *)PInicio,strlen(PInicio),HAL_MAX_DELAY);
  HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(p==1)
	  {
			 /* if(ModoJ==0)
			  {
				  HAL_UART_Transmit_IT(&huart2, (uint8_t *)PInicio,sizeof(PInicio));
			  }*/
		 if(ModoJ==1)
		 {
				  switch(tipo)
				  {
				  case 1:

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg1,sizeof(msg1));
					  tipo=0;
					 break;

				  case 2:

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg2,sizeof(msg2));
					  tipo=0;
					  break;

				  case 3:

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg3,sizeof(msg3));
					  tipo=0;
				  break;

				  case 4:

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg4,sizeof(msg4));
					  tipo=0;
				  break;
				  case 5:

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg5,sizeof(msg5));
					  tipo=0;
				  break;

				  }
		}
		if(ModoJ==2)
		{
				  switch(tipo)
				  {
				  case 1:
					  HAL_UART_Transmit(&huart2, (uint8_t *)msg1,strlen(msg1),1);
					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)MJ21,sizeof(MJ21));
					  tipo=0;
				  break;
				  case 2:
					  HAL_UART_Transmit(&huart2, (uint8_t *)msg2,strlen(msg2),1);
					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)MJ21,sizeof(MJ21));
					  tipo=0;
				  break;
				  case 3:
					  HAL_UART_Transmit(&huart2, (uint8_t *)msg3,strlen(msg3),1);
					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)MJ21,sizeof(MJ21));
					  tipo=0;
				  break;
				  }
			  }
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
