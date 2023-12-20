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
#include <math.h>
#include <unistd.h> // obtener funcion getpid()
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <PWM_TIM3.h>
#include <PWM_TIM4.h>
#include <MODO_TIC_TIM2.h>
#include <stream_TIM1.h>
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
//---------COMBINACIÓN ALEATORIOA--------------------//

int aleatorio1=0; // numero generado por random
int aleatorio2=0; // numero generado por random
float final=0; // residuo de la division
int final2=0; // residuo de la division
int LimiteRand = 6; // se obtienen aleatorios de 1 a 6
int LimiteRand1 = 5; // se obtienen aleatorios de 1 a 5

//-----------CONTROL DISPLAY-----------------------//

uint8_t up=2;
uint8_t contador = 9;
uint8_t display[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
uint8_t cero=0;
//-----------------------------------------------------//
uint8_t juego=0;
uint8_t s=0; //Elegir numero de rondas
uint8_t rondas=0; //Numero de rondas a jugar
uint8_t ModoJ=0; //Seleccionar modo de juego 1 o 2 personas
uint8_t tipo=0; // Tipo de juego
uint8_t p=0; //print
uint8_t inicio=0; //iniciar juego
uint8_t B1=0; //Variable control botón pulsado
uint8_t B2=0; //Variable control botón pulsado
uint32_t IC_CONTEO1=0;
uint32_t IC_CONTEO2=0;

char msg[]="\r\n Bienvenido al juego de reccion, en este juego se pondran a prueba \r\n"
" tus reflejos con alertas visuales o sonoras \r\n";//mensaje de bienvenida
char PInicio[]="Teclee el numero corresponiende al modo de juego \r\n"
"Seleccione el modo de juego \r\n 1.-Un jugador \r\n 2.-Dos jugadores \r\n";
char MJ1[]="Entro en modo de un jugador:\r\n"
"A.- Reaccion a un estimulo \r\nB.- Nocion del tiempo \r\n 0.- Regresar\r\n";
char MJ2[]="Entro en el modo de dos jugadores:\r\n"
"A.- Reaccion a un estimulo \r\nB.- Nocion del tiempo\r\n0.-Regresar\r\n";
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
				TIM4->CR1 &=~(1<<0);
				TIM4->CCER &=~(1<<4);
				TIM3->CR1 &=~(1<<0);
				TIM3->CCER &=~(1<<8);
				TIM1->CR1 &=~(1<<0);
				TIM1->CNT=0;
				Display_Port(0x00); //APAGAMOS EL DISPLAY 7 SEGMENTOS
				contador=9;
				cero=0;
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
				contador=4;
				cero=0;
				tipo=1;
			}
			if(!strcmp(Rx_data,"B"))
			{
				contador=9;
				cero=0;
				tipo=2;
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

void TIM2_IRQHandler(void) //INTERRUPCIÓN DEL TIMER DE MEDICIÓN
{


	if((TIM2->SR & 1)) //Se ha desbordado. Se acabó el tiempo
	{
	//	PUL=1;
		TIM2->CR1 &= ~(1<<0); //Apaga timer. No se ha producido captura
		TIM2->SR &= ~(1<<0); // Limpio los flags del contador
	}
	if ((TIM2->SR & (1<<1)))
	{ 	// Se produce una captura TIC

				IC_CONTEO1 = TIM2->CCR1; // Se tomo el número de Tics <CANAL 1>
				B1=1; // Actualizo para que el programa principal
				TIM2->SR &= ~(1<<1); // Limpio los flags del contador

	}
	if ((TIM2->SR & (1<<2)))
	{ 	// Se produce una captura TIC

		B2=1; // Actualizo para que el programa principal
				IC_CONTEO2 = TIM2->CCR2; // Se tomo el número de Tics <CANAL 2>
				TIM2->SR &= ~(1<<2); // Limpio los flags del contador

	}

}

void Display_Port(uint8_t data)
{ //ESTA FUNCION EXTRAE EL BIT CORRESPONDIENTE AL PIN O SEGMENTO PARA VER SI ES UN "1" O UN "0" Y ENCENDER EL SEGMENTO CORRESPONDIENTE. Multiplica la máscara por el bit correspondiente y lo desplaza a la derecha para ver si es un 1 o un o
	HAL_GPIO_WritePin(a_GPIO_Port, a_Pin, (((data & 0x01) >> 0) & 0x01));
	HAL_GPIO_WritePin(b_GPIO_Port, b_Pin, (((data & 0x02) >> 1) & 0x01));
	HAL_GPIO_WritePin(c_GPIO_Port, c_Pin, (((data & 0x04) >> 2) & 0x01));
	HAL_GPIO_WritePin(d_GPIO_Port, d_Pin, (((data & 0x08) >> 3) & 0x01));
	HAL_GPIO_WritePin(e_GPIO_Port, e_Pin, (((data & 0x10) >> 4) & 0x01));
	HAL_GPIO_WritePin(f_GPIO_Port, f_Pin, (((data & 0x20) >> 5) & 0x01));
	HAL_GPIO_WritePin(g_GPIO_Port, g_Pin, (((data & 0x40) >> 6) & 0x01));
}

void TIM1_UP_TIM10_IRQHandler() //TIMER DE CONTROL DE IMPRIMIR POR PANTALLA
{
	if(TIM1->SR & (1<<0)) //Puede ser la IRQ del TIM10 también, nos ASEGURAMOS que es el TIM1 (aunque esté deshabilitado el TIM10)
	{
		switch(up)
		{
		case 1:
			contador++;
			if(contador == 9)
			{

				TIM1->CR1 &=~(1<<0);
			}
			break;

		case 2:
			cero++;
			contador--;
			if(cero == 10)
			{
				TIM1->CR1 &=~(1<<0);
			}
			break;
		}
	}
	TIM1->SR=0; //RESET interrupción

}

void clk_enable()
{
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
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
	uint8_t ronda=0;//Variable control while
	float Tiempo1=0;
	float Tiempo2=0;
	float mejora=10; //variable que almacena el mejor tiempo, EMPIEZA EN 10S
	float mejora_tiempo=10;


	char res_string[75]="";  //modo 2 jugadores
	char res_string1[40]=""; //modo 1 jugador (entrenamiento)
	char msg1[]="Reaccion a un estimulo (MODO ENTRENAMIENTO)\r\n"
"Cuando suene este sonido o se ilumine el led azul PULSE EL BOTON\r\n";
	char msg12[]="Reaccion a un estimulo \r\n";
	char msg2[]="Nocion del tiempo\r\n";
	char msg5[]="FELICIDADES!! Mejoraste tu tiempo de reaccion\r\n";
	char msg6[]="No superaste tu anterior tiempo, sigue intentandolo!!\r\n";


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  clk_enable();
  PWM();
  PWM4();
  TIMER_MED_TIC();
  stream_TIM();
 // stream_TIM();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

 //------------------MENSAJE DE INICIO DEL JUEGO------------------------------//

  HAL_UART_Transmit(&huart2, (uint8_t *)msg,strlen(msg),HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart2, (uint8_t *)PInicio,strlen(PInicio),HAL_MAX_DELAY);
  HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);

 //--------------------------------------------------------------------------//
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
{

//--------------------------------GENERACIÓN DE NÚMEROS ALEATORIOS--------------------------//

//******************NUMEROS ALEATORIOS PARA LA COMBINACIÓN DEL DISPLAY*************************//

		aleatorio1 = rand(); //general el numero aleatorio
		final = (aleatorio1%LimiteRand)+2; //obtiene el valor final



//********************************************************************************************//

	  if(p==1)
	  {

		 if(ModoJ==1)// MODO 1 JUGADOR
		 {
				  switch(tipo)
				  {
				  case 1: //Reaccion a un estimulo
					  	  ronda=0;
					  juego=1;
					  	  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg1,sizeof(msg1));
					  	  TIM4->CR1 |=(1<<0);//Activar timmer
					  	  TIM4->CCER |=(1<<4);//Activar PWM4
					  	  TIM1->CR1 |=(1<<0);
					  	  while(cero<5)
					  	  {
					  		  Display_Port(display[contador]); //CONTADOR DEL DISPLAY
					  	  }
					  	  TIM4->CR1 &=~(1<<0); //desactivar Timer
					  	  TIM4->CCER &=~(1<<4); //Desactivar buzzer
					  	  HAL_Delay(500);

//-------------FUNCIONES QUE VAN A OCURRIR DURANTE EL JUEGO-----------------------------//

					  	  TIM2->CR1 |=(1<<0); //timer CAPTURA on
					  	  TIM2->CCER |=0x0011; //ANTES de que ocurra el estímulo NO se tendrá en cuenta ningún resultado

					  	  while(ronda==0)
					  	  {
						//***************************NUMEROS ALEATORIOS PARA LA COMBINACIÓN DE ESTIMULOS***************//

					  		  aleatorio2 = rand();
					  		  final2=(aleatorio2%LimiteRand1)+1;

					  		  switch (final2)
					  		  {
					  		  case 1:
					  			  GPIOB->BSRR |=(1<<5);//ENCIENDE LED ROJO
					  			  HAL_Delay(500);
					  			  GPIOB->BSRR |=((1<<5)<<16); //APAGA LED ROJO

					  			  break;

					  		  case 2:
					  			  TIM3->CCER |=(1<<8);// ACTIVAMOS EL BUZZER FALSO
					  			  HAL_Delay(500);
					  			  TIM3->CCER &=~(1<<8);// DESACTIVAMOS EL BUZZER FALSO

					  			  break;

					  		  case 3:
					  			  GPIOB->BSRR |=(1<<4);//ENCIENDE LED AMARILLO
					  			  HAL_Delay(500);
					  			  GPIOB->BSRR |=((1<<4)<<16); //APAGA LED AMARILLO

					  			  break;

					  		  case 4:
					  			  GPIOB->BSRR |=(1<<3);//ENCIENDE LED AZUL

					  			  ronda=1;
					  			  HAL_Delay(500);
					  			  break;

					  		  case 5:
					  			  TIM4->CCER |=(1<<4);//Activar BUZZER 2

					  			  ronda=1;
					  			  HAL_Delay(500);
					  			  break;
						default:
							GPIOB->BSRR |=(1<<5);//ENCIENDE LED ROJO
							HAL_Delay(500);
							GPIOB->BSRR |=((1<<5)<<16); //APAGA LED ROJO

							break;
						}

					}

					  tipo=0;
//-----------------------FIN DEL JUEGO-------------------------------///
					 break;

				  case 2: //Nocion del tiempo

					  HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg2,sizeof(msg2));
					  TIM1->CR1 |=(1<<0);
					  while(cero<final)
					  {
						  Display_Port(display[contador]); //CONTADOR DEL DISPLAY
					  }
					  TIM2->CR1 |=(1<<0); //timer on
					  TIM2->CCER |=0x0001;
					  juego=2;
					  tipo=0;
					  break;

				  }
		}
		if(ModoJ==2)
		{// UNA IDEA PARA EL NUMERO DE RONDAS ES USAR FUNCION "HAL_UART_RECEIVE_IT CON HAL MAX DELAY" PARA QE EL PROGRAMA ESTÉ CONSTANTEMENTE
			//ESPERANDO POR EL CARACTER Y HASTA QUE NO LE INTRODUZCA EL VALOR NO CONTINUAR�? EN EL PROGRAMA
				  switch(tipo)
				  {
				  case 1: // Reaccion a un estimulo

					  ronda=0;
					  HAL_UART_Transmit(&huart2, (uint8_t *)msg12,strlen(msg12),2);

					  TIM4->CR1 |=(1<<0);//Activar timmer
					  TIM4->CCER |=(1<<4);//Activar PWM4
					  TIM1->CR1 |=(1<<0);

					  while(cero<5)
					  {
						  Display_Port(display[contador]); //CONTADOR DEL DISPLAY
					  }
					  TIM4->CR1 &=~(1<<0);
					  TIM4->CCER &=~(1<<4);
					  HAL_Delay(500);

					//-------------FUNCIONES QUE VAN A OCURRIR DURANTE EL JUEGO-----------------------------//
					  TIM2->CR1 |=(1<<0); //timer CAPTURA on
					  TIM2->CCER |=0x0011; //ANTES de que ocurra el estímulo NO se tendrá en cuenta ningún resultado
					  juego=1;
					while(ronda==0)
						{
							//***************************NUMEROS ALEATORIOS PARA LA COMBINACIÓN DE ESTIMULOS***************//
							aleatorio2 = rand();
							final2=(aleatorio2%LimiteRand1)+1;

							switch (final2)
							{
							case 1:
								GPIOB->BSRR |=(1<<5);//ENCIENDE LED ROJO
								HAL_Delay(500);
								GPIOB->BSRR |=((1<<5)<<16); //APAGA LED ROJO

							break;

							case 2:
								TIM3->CCER |=(1<<8);// ACTIVAMOS EL BUZZER FALSO
								HAL_Delay(500);
								TIM3->CCER &=~(1<<8);// DESACTIVAMOS EL BUZZER FALSO
								break;

							case 3:
								GPIOB->BSRR |=(1<<4);//ENCIENDE LED AMARILLO
								HAL_Delay(500);
								GPIOB->BSRR |=((1<<4)<<16); //APAGA LED AMARILLO
								break;

							case 4:
								GPIOB->BSRR |=(1<<3);//ENCIENDE LED AZUL
								ronda=1;
								HAL_Delay(1000);
							break;

							case 5:
								 TIM4->CCER |=(1<<4);//Activar BUZZER 2
								 ronda=1;
								 HAL_Delay(500);
								 break;
							default:
							GPIOB->BSRR |=(1<<5);//ENCIENDE LED ROJO
							HAL_Delay(500);
							GPIOB->BSRR |=((1<<5)<<16); //APAGA LED ROJO

								break;
							}
						}

					  tipo=0;
 //-------------------------------FIN DEL JUEGO 1----------------------------------///
				  break;
				  case 2: // Nocion del tiempo

					  HAL_UART_Transmit(&huart2, (uint8_t *)msg2,strlen(msg2),2);
					  TIM1->CR1 |=(1<<0);
					  while(cero<final)
					  {
						  Display_Port(display[contador]); //CONTADOR DEL DISPLAY
					  }
					  TIM2->CR1 |=(1<<0); //timer on
					  TIM2->CCER |=0x0011;
					  juego=2;
					  tipo=0;
				  break;
				  }
			  }

		if(B1==1&&ModoJ==1&&juego==1) //SOLO EN MODO 1 JUGADOR (estímulo)
		{
			ronda=1;
			B1=0;
			Tiempo1 = (float) IC_CONTEO1/10000; // Escalamos para pasar el tiempo de microsegundos a segundos

				sprintf(res_string1, "Tiempo de reaccion del jugador: %.2f\r\n",Tiempo1);
				HAL_UART_Transmit(&huart2, (uint8_t *)res_string1,strlen(res_string1),5);
				if(mejora>Tiempo1)
				{
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg5,sizeof(msg5));
					mejora=Tiempo1;
				}else
				{
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg6,sizeof(msg6));
				}

			Tiempo1=0;
			GPIOB->BSRR |=((1<<3)<<16); //APAGA LED AZUL
			TIM4->CCER &=~(1<<4);// DESACTIVAMOS EL BUZZER 2
			TIM2->CR1 &=~(1<<0); //El boton 1 ha sido pulsado se APAGA el timer
			TIM2->CCER &=~0x0011; //Se desactiva el modo captura
			TIM2->CNT=0; //REINICIAMOS LA CUENTA
			juego=0;
		}

		if(B1==1&&ModoJ==1&&juego==2) //SOLO EN MODO 1 JUGADOR (Nocion tiempo)
		{
			B1=0;

			Tiempo1 = (float) IC_CONTEO1/10000; // Escalamos para pasar el tiempo de microsegundos a segundos
			Tiempo1=fabs((final-Tiempo1));

			sprintf(res_string1, "Tiempo de reaccion del jugador: %.2f\r\n",Tiempo1);
			HAL_UART_Transmit(&huart2, (uint8_t *)res_string1,strlen(res_string1),5);

			if(mejora_tiempo>Tiempo1)
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg5,sizeof(msg5));
				mejora_tiempo=Tiempo1;
			}else
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *)msg6,sizeof(msg6));
			}
			Tiempo1=0;
			TIM2->CR1 &=~(1<<0); //Los 2 botones han sido pulsados se APAGA el timer
			TIM2->CCER &=~0x0001; //Se desactiva el modo captura
			TIM2->CNT=0; //REINICIAMOS LA CUENTA
			 juego=0;
		}

		if(B1==1&&B2==1&&ModoJ==2) //SE ACTIVA EN EL MODO 2 JUGADDORES (MODO ESTíMULO)
		{
			B1=0;
			B2=0;

				if(IC_CONTEO1<IC_CONTEO2)
				{
					Tiempo1 = (float) IC_CONTEO1/10000; // Escalamos para pasar el tiempo de microsegundos a segundos
					Tiempo2 = (float) IC_CONTEO2/10000;
					sprintf(res_string, "Jugador 1 ha ganado, <tiempo jugador1: %.2fs  <tiempo jugador2: %.2fs> \r\n", Tiempo1,Tiempo2);
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)res_string,sizeof(res_string));
				}else
				{
					Tiempo1 = (float) IC_CONTEO1/10000; // Escalamos para pasar el tiempo de microsegundos a segundos
					Tiempo2 = (float) IC_CONTEO2/10000;
					sprintf(res_string, "Jugador 2 ha ganado, <tiempo jugador1: %.2fs>  <tiempo jugador2: %.2fs> \r\n", Tiempo1,Tiempo2);
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)res_string,sizeof(res_string));
				}

			Tiempo1=0;
			Tiempo2=0;
			GPIOB->BSRR |=((1<<3)<<16); //APAGA LED AZUL
			TIM4->CCER &=~(1<<4);// DESACTIVAMOS EL BUZZER 2
			TIM2->CR1 &=~(1<<0); //El boton 1 ha sido pulsado se APAGA el timer
			TIM2->CCER &=~0x0011; //Se desactiva el modo captura
			TIM2->CNT=0; //REINICIAMOS LA CUENTA
			 juego=0;

		}
		if(B1==1&&B2==1&&ModoJ==2) //SE ACTIVA EN EL MODO 2 JUGADORES (NOCIÓN TIEMPO)
				{
					B1=0;
					B2=0;
						Tiempo1 = (float) IC_CONTEO1/10000; // Escalamos para pasar el tiempo de microsegundos a segundos
						Tiempo2 = (float) IC_CONTEO2/10000;
					Tiempo1=fabs((final-Tiempo1));
					Tiempo2=fabs((final-Tiempo2));
					if(Tiempo1<Tiempo2)
					{
					sprintf(res_string, "Jugador 1 ha ganado, <tiempo jugador1: %.2fs  <tiempo jugador2: %.2fs> \r\n", Tiempo1,Tiempo2);
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)res_string,sizeof(res_string));
					}else
					{
					sprintf(res_string, "Jugador 2 ha ganado, <tiempo jugador1: %.2fs  <tiempo jugador2: %.2fs> \r\n", Tiempo1,Tiempo2);
					HAL_UART_Transmit_IT(&huart2, (uint8_t *)res_string,sizeof(res_string));
					}
					Tiempo1=0;
					Tiempo2=0;
					TIM2->CR1 &=~(1<<0); //Los 2 botones han sido pulsados se APAGA el timer
					TIM2->CCER &=~0x0011; //Se desactiva el modo captura
					TIM2->CNT=0; //REINICIAMOS LA CUENTA
					 juego=0;

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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, a_Pin|b_Pin|c_Pin|d_Pin
                          |e_Pin|f_Pin|g_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_AZUL_Pin|LED_AMARILLO_Pin|LED_ROJO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PLAYER_1_Pin PLAYER_2_Pin */
  GPIO_InitStruct.Pin = PLAYER_1_Pin|PLAYER_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : a_Pin b_Pin c_Pin d_Pin
                           e_Pin f_Pin g_Pin */
  GPIO_InitStruct.Pin = a_Pin|b_Pin|c_Pin|d_Pin
                          |e_Pin|f_Pin|g_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER1_Pin */
  GPIO_InitStruct.Pin = BUZZER1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(BUZZER1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_AZUL_Pin LED_AMARILLO_Pin LED_ROJO_Pin */
  GPIO_InitStruct.Pin = LED_AZUL_Pin|LED_AMARILLO_Pin|LED_ROJO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER2_Pin */
  GPIO_InitStruct.Pin = BUZZER2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(BUZZER2_GPIO_Port, &GPIO_InitStruct);

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
