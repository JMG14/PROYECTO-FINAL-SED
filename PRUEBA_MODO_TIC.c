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
#include "stdio.h"
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

char WELCOME_MSG[]="DIEGO muerdealmohadas\n\r";
uint8_t estado=1;
uint8_t PUL=0;
uint8_t TON=0;
uint32_t IC_CONTEO=0;
uint32_t IC_CONTEO1=0;
char msg[70]=""; //con la función <sprintf> MUY IMPORTANTE definir el número de espacios necesarios en el array para
//poder guardar la cadena de caracteres con éxito sino no imprime NADA por pantalla o da error
float Tiempo=0;
float Tiempo1=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void TIMMER()  //----------------------TIMER DE MEDICIÓN TIC-----------------------//
{
	// Modo de salida del contador
	TIM2->CR1 = 0;
	TIM2->CR2 = 0;
	TIM2->SMCR = 0;

	TIM2->CCMR1 =0;
	TIM2->CCMR1 |=0xC1C1;

	TIM2->CCER=0;
	TIM2->CCER |=0x0011;

    /*TIM2->CCMR1 |=(01<<0);//configuración <CANAL 1> como INPUT (TIC)
    TIM2->CCMR1 |= (1100<<4); //no usamos preescaler pero se usa filtro <CANAL 1>
    TIM2->CCER |=(1<<0); //ACTIVAR el modo CAPTURA (TIC) del <CANAL 1>
    TIM2->CCER &=~(11<<1); //Activo por flanco de BAJADA


	TIM2->CCMR1 |=(01<<8); //configuración <CANAL 2> como INPUT (TIC) vamos a MEDIR señal tiempo de "ECHO"
	TIM2->CCMR1 |= (1100<<12); //no usamos preescaler pero se usa filtro <CANAL 2>
	TIM2->CCER |= (1<<4); // CCyNP:CCyP=01 (activo a f. caída) CCyE=1 (captura habilitada-TIC) <CANAL 2>
	TIM2->CCER &=~(11<<5); //Activo por flanco de SUBIDA*/


	TIM2->PSC = 83; //Define el PSC. Tu=1us
	TIM2->ARR = 9999999; //Por defecto se carga al mayor valor posible. Esto no es necesario

	//Listos Go
	TIM2->EGR |= (1<<0); //Si lo hacemos genera un Update....NO HACERLO antes de tiempo! Aquí si
	TIM2->SR=0;
	//TIM2->CR1 |= (1<<0 );//Todavía no arrancamos el timer. Lo hará el EXTI13.

	//interrupciones. Va después de actualizar para evitar que salte la IRQ del TMR
	TIM2->DIER |= (1<<0)|(1<<1)|(1<<2); // Se genera INT al ocurrir un CCyIE=1 o UIF=1

}
void TIMER3_CONFIG()  //-----------------------ESTE TIMER SE ENCARGA DE HACER EL ANTIREBOTE (DEBOUCING)---------//
{
	// Modo de salida del contador
	TIM3->CR1 = 0;
	TIM3->CR2 = 0;
	TIM3->SMCR = 0;
	TIM3->CCMR1 = 0; // CCyS = 1 (TIC); OCyM = 000 y OCyPE = 0 (TIC)
	TIM3->CCER = 0; // CCyNP:CCyP = 01 (activo a flanco de caída) CCyE=1 (captura habilitada para TIC)

	//Definimos la BT
	TIM3->PSC = 8399; //Define el PSC. Tu=0.1ms
	TIM3->ARR = 2500;//Por defecto se carga al mayor valor posible. Esto no es necesario
	//Listos Go
	TIM3->EGR |= (1<<0); //Si lo hacemos genera un Update....NO HACERLO antes de tiempo! Aquí si
	TIM3->SR=0;
	//TIM3->CR1 |= (1<<0 );//Todavía no arrancamos el timer. Lo hará el EXTI13.

	//¿Cómo serán las interrupciones? Va después de actualizar para evitar que salte la IRQ del TMR antes de lo previsto
	TIM3->DIER |= (1<<0 ); // Se genera INT al ocurrir un CCyIE=1 o UIF=1
	NVIC->ISER[0] |= (1<<29); //NVIC_EnableIRQ(TIM3_IRQn) Habilitamos TIM3 IRQn
}
void clk_cnfig()
{
	__HAL_RCC_GPIOB_CLK_ENABLE(); //Habilitamos reloj puerto B
	RCC->AHB1ENR |= ((1<<0)|(1<<2));// puerto A y puerto C
	RCC->APB1ENR |= ((1<<0)|(1<<1)); // Reloj TIM2/TIM3
}
void EXTII_CNFIG()
{
	SYSCFG->EXTICR[3] &= (1111 << 4);// reset
	SYSCFG->EXTICR[3] |= ( 1 << 5); //Habilita interrupcion puerto C
	EXTI->IMR |= (1 << 13);
	EXTI->RTSR &= ~(1 << 13);//reset
	EXTI->FTSR |= (1 << 13);//flanco bajada activado
	//NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);

}
void GPIO_CONF(void){


	GPIOA->MODER |= (01<<(5*2)); //PA5 como Salida
	GPIOA->OTYPER &= ~(1<<5); //PA5 PusPull

	GPIOA->MODER |= ((1<<(2*2+1)) | (1<<(3*2+1))); //Configura en modo AF el PA3 y el PA2

/*//-----------------CONFIGURACION PERIFÉRICOS MODO TIC--------------------------//
	GPIOA->MODER &= ~(11 << (1*2)); // reset de PA1 por este pin se realizará la medición de TIEMPO enviada por el sensor
	GPIOA->MODER |=(10<<(1*2));//Configuramos como AF el pin PA1
	GPIOB->AFR[0] &=~(1111<<(1*4));//Reset de las posiciones
	GPIOB->AFR[0] |=(1<<(1*4));//Configuración de AF01 para el PA1 relacionado con CH2 del TIM2*/
//------------------CONFIGURACION USART-----------------------------//
	GPIOA->AFR[0]|=(7 << (2*4)); // AFR[0] PA2 --> AF7 (TX USART2)
	GPIOA->AFR[0]|=(7 << (3*4)); // AFR[0] PA3 --> AF7 (RX USART2)

}

void EXTI15_10_IRQHandler(void) {
  if (EXTI->PR & (1<<13)&&(estado==1))
  {
	  TIM3->CR1 |= (1<<0); //arrancamos timer
	  estado = 0;
  }
  else{
	  __NOP (); //descartamos triggers ante de 250ms
  }
EXTI->PR |= (1 << 13); // Clear interrupt
}

void TIM3_IRQHandler(void)
{
    TIM3->CR1 &= ~(1<<0); //apagamos timer
    if (TON==0)
 	 	 {
	 	 	 TIM2->CR1 |= (1<<0); //Enciende TIM2
	 	 	 GPIOA->BSRR |= (1<<5); //Enciende LED
	 	 	 PUL=1;
	 	 	 TON=1;
  	  	  }
    else
    		{
	 	 	 TIM2->CR1 &=~(1<<0); //Apaga TIM2.Voluntad usuario
	 	 	 GPIOA->BSRR|=(1<<5)<<16; //Apaga LED
	 	 	 PUL=2;
	 	 	 TON=0;
    		}
    estado=1;
    TIM3->SR &= ~(1<<0); // Limpio los flags del contador
}

void TIM2_IRQHandler(void){


	if((TIM2->SR & 1)) //Se ha desbordado. Se acabó el tiempo
	{
		PUL=3;
		TIM2->CR1 &= ~(1<<0); //Apaga timer. No se ha producido captura
		GPIOA->BSRR |= (1<<5)<<16; //Apaga LED
		TON=0;
		TIM2->SR &= ~(1<<0); // Limpio los flags del contador
	}
	if ((TIM2->SR & (1<<2)))
	{ 	// Se produce una captura TIC

		PUL=4; // Actualizo para que el programa principal
				IC_CONTEO = TIM2->CCR2; // Se tomo el número de Tics <CANAL 2>
				TIM2->SR &= ~(1<<2); // Limpio los flags del contador

	}
	if ((TIM2->SR & (1<<1)))
	{ 	// Se produce una captura TIC
		//GPIOA->BSRR |=(1<<5);
		PUL=4; // Actualizo para que el programa principal
				IC_CONTEO1 = TIM2->CCR1; // Se tomo el número de Tics <CANAL 1>
				TIM2->SR &= ~(1<<1); // Limpio los flags del contador

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
  clk_cnfig();
    GPIO_CONF();
    EXTII_CNFIG();
    TIMMER();
    //NVIC->ISER[0] |= (1 << 30);// ACTIVAR INTERRUPCION TIMER 4
    NVIC->ISER[0] |= (1<<28); //NVIC_EnableIRQ(TIM2_IRQn) Habilitamos TIM2 IRQn
    TIMER3_CONFIG();
    HAL_UART_Transmit(&huart2, (uint8_t*) WELCOME_MSG, strlen(WELCOME_MSG), HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
{

	  if (PUL==1)
	  {
	  PUL=0; // Si pulso lo pongo a 0 para la próxima interrupción
	  sprintf(msg, "\r\nTimer encendido por usuario\r\n");
	  HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
	  }
	  if (PUL==2)
	  {
	  PUL=0; // Si pulso lo pongo a 0 para la próxima interrupción
	  char msg[]= "\r\nTimer apagado por usuario\r\n";
	  HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);}
	  if (PUL==3)
	  {
	  PUL=0; // Si pulso lo pongo a 0 para la próxima interrupción
	  char msg[]= "\r\nTimer autoapagado (>10s)\r\n";
	  HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);}
	  if (PUL==4)
	  {
	  PUL=0; // Si pulso lo pongo a 0 para la próxima interrupción
	  Tiempo = (float) IC_CONTEO/1000000; // Escalamos para pasar el tiempo de microsegundos a segundos
	  Tiempo1 = (float) IC_CONTEO1/1000000;
	  sprintf(msg, "Captura canal 1 en: %.2f segundos y captura canal 2: %.2f segundos\r\n", Tiempo1,Tiempo);
	  HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
