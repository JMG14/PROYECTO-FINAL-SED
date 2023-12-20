#include <main.h>

void PWM4()
{
  //******************************BT TIMER4****************************************//

	TIM4->CNT=0;
	TIM4->PSC=8400-1; // Tu=0,1ms
	TIM4->ARR=18-1; //  10ms     SEÑAL DE 1KHz PERÍODO //500HZ 20ms //2000Hz 5ms

  //------------------------CONFIGURACIONES INTERNAS-------------------------------//

	TIM4->CR1=0;//RESET
	TIM4->CR1 |=(1<<2)|(1<<7); //Habilitar UDIS solo evento update y Habilitar ARPE para cambiar CCRy
	TIM4->CR2=0; //Deshabilitado
	TIM4->SMCR=0; //Deshabilitado

  //------------------------CONFIGURACIONES MODO TOC-------------------------------//

	TIM4->CCMR1 &=~(11<<8); //Modo TOC <CANAL 2>
	TIM4->CCMR1 &=~(6<<12); // PWM modo 1 Activo a nivel ALTO <CANAL 2>
	TIM4->CCMR1 |=(6<<12); // PWM modo 1 Activo a nivel ALTO <CANAL 2>

	TIM4->CCER &=~(11<<4); //RESET y dejamos por defecto que CCRy se active a nivel ALTO
	//TIM4->CCER |=(1<<4);// HABILITAMOS la PWM <CANAL 1>

	TIM4->CCR2=14;
/*
	TIM4->CCMR2 &=~(11<<8); //Modo TOC <CANAL 4>
	TIM4->CCMR2 &=~(6<<12); // PWM modo 1 Activo a nivel ALTO <CANAL 4>
	TIM4->CCMR2 |=(6<<12); // PWM modo 1 Activo a nivel ALTO <CANAL 4>

	TIM4->CCER &=~(1<<12); //RESET y dejamos por defecto que CCRy se active a nivel ALTO
	//TIM4->CCER |=(1<<12);// HABILITAMOS la PWM  <CANAL 4>

	TIM4->CCR4=50-1; 	// SEÑAL PWM2 con DC=25%
*/
  //*************************INTERRUPCIONES*****************************************//

	TIM4->DIER=0; //RESET
	TIM4->DIER |=(1<<0); //IRQ update ACTIVADA
	TIM4->EGR=0; //RESET
	TIM4->EGR |=(1<<0);// UPDATE de los registros

	//NVIC_EnableIRQ(TIM4_IRQn); //avisamos al NVIC de la interrupción UPDATE

  //***********************LISTOS GO!!*********************************************//

	TIM4->SR&=~(1<<0); //RESET FLAG
	//TIM4->CR1 |=(1<<0);
}
