
#include <main.h>

void TIMER_MED_TIC()
{
 //---------CONFIGURACIÓN DE <BT> DEL TIMER 2---------------------------//
//tiempo 10s de reacción
	TIM2->ARR = 600000-1; // Nuestro MODO TIC estará funcionando durante 1min que dure el juego
    TIM2->PSC =8400-1; //tengo una resolución Tu=0.1ms
	TIM2->CNT =0; //la cuenta empieza en 0

//------------------CONFIGURACIÓN INTERNA DEL TIMER 2--------------------------------------//

	TIM2->CR1 =0;//Desactivamos TODAS las fnciones de CR1
	TIM2->CR2=0;//Desactivamos TODAS las funciones de CR2
	TIM2->SMCR =0; //DESACTIVADO el modo esclavo

//---------CONFIGURACIÓN MODO TIC PARA CAPTURAR LA SEÑAL DE TIEMPO---------------------------//

	TIM2->CCMR1 =0;
	TIM2->CCMR1 |=0xC1C1; // <canal 1> y <canal 2> con filtro aplicado

	TIM2->CCER=0;
	//TIM2->CCER |=0x0011; //Habilitación del modo captura y en detección flanco de subida

//----------------ACTIVACIÓN FINAL DEL TIMER-------------------------//

	TIM2->EGR = 0;
	TIM2->EGR |=(1<<0);//actualizacion de eventos GENERAMOS un UPDATE antes de activar la interrupción para que no genere
	//ninguna señal antes de tiempo
	TIM2->SR =0; //limpiar el flag
	TIM2->DIER =0;//RESET
	TIM2->DIER |=((1<<0)|(1<<2)|(1<<1));// Se genera una interrupción al ocurrir una CAPTURA o un UPDATE <CANAL 2>

	//TIM2->CR1 |=(1<<0); //timer on
	NVIC_EnableIRQ(TIM2_IRQn); //habilitamos interrupción del TIMER2

}
