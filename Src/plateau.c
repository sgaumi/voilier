//plateau.c
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges
#include "stm32f1xx_ll_tim.h"  // Pour le timer

#include "bordage.h" //pour abso

void gpio_RF_init(){

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	//PB6
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_FLOATING); //a determiner (alternate?)
	//PB7
	//LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_FLOATING); //a determiner


}

void timer_RF_init(){

	//init timer
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	
	LL_TIM_InitTypeDef timer_init;
	timer_init.Autoreload=999;
	timer_init.Prescaler=719;
	timer_init.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	timer_init.CounterMode=LL_TIM_COUNTERMODE_UP;
	timer_init.RepetitionCounter=0;
	
	LL_TIM_Init(TIM4,&timer_init);
	
	//Select the active input for TIMx_CCR1: write the CC1S bits to 01 in the TIMx_CCMR1 register (TI1 selected).
	TIM4->CCMR1 &= ~(0x0003);
	TIM4->CCMR1 |= 0x0001;
	
	//Select the active polarity for TI1FP1 (used both for capture in TIMx_CCR1 and counter clear): write the CC1P to ‘0’  (active on rising edge).
	TIM4->CCER &= ~(0x0002);
	
	//Select the active input for TIMx_CCR2: write the CC2S bits to 10 in the TIMx_CCMR1 register (TI1 selected).
	TIM4->CCMR1 &= ~(3<<8);
	TIM4->CCMR1 |= 2<<8;
	
	//Select the active polarity for TI1FP2 (used for capture in TIMx_CCR2): write the CC2P bit to ‘1’ (active on falling edge)
	TIM4->CCER |= 1<<5;
	
	//Select the valid trigger input: write the TS bits to 101 in the TIMx_SMCR register (TI1FP1 selected).
	TIM4->SMCR &= ~(7<<4);
	TIM4->SMCR |= 5<<4;
	
	//Configure the slave mode controller in reset mode: write the SMS bits to 100 in the TIMx_SMCR register.
	TIM4->SMCR &= ~(0x0007);
	TIM4->SMCR |= 0x0004;
	
	//Enable the captures: write the CC1E and CC2E bits to ‘1 in the TIMx_CCER register.
	TIM4->CCER |= 0x0001;
	TIM4->CCER |= 1<<4;
	
	
	
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void initializeGPIO() {
	
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);  //GPIOA sur APB2
	//configuration de la PA1 pour générer la PWM
	//pwm PA1 tim 2 ch2
	LL_GPIO_InitTypeDef pa1;
	pa1.Pin = LL_GPIO_PIN_1;
	pa1.Mode = LL_GPIO_MODE_ALTERNATE;
	pa1.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa1.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa1.Pull = LL_GPIO_PULL_UP ;

	
}		
	
void initializeTimerPWM() {
	
	int Arr = 65454 ;
	int Psc = 54 ;

	LL_TIM_InitTypeDef Timer;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	//timer correspondant à la PWM
	Timer.Autoreload= Arr;
	Timer.Prescaler= Psc;
	Timer.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	Timer.CounterMode=LL_TIM_COUNTERMODE_UP;
	Timer.RepetitionCounter=0;
	
	LL_TIM_OC_SetMode (TIM1, LL_TIM_CHANNEL_CH1,LL_TIM_OCMODE_PWM1);
	//rajouter les deux lignes des flags
	
}	
	
	
	
void initializeSens() {
	//configuration de la PA2 qui gère le sens (en 0 ou 1)
	//sens PA2 tim 2 ch3
	LL_GPIO_InitTypeDef pa2;
	pa2.Pin = LL_GPIO_PIN_2;
	pa2.Mode = LL_GPIO_MODE_INPUT;
	pa2.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa2.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa2.Pull = LL_GPIO_PULL_UP ;
}
	
	
void bougerPlateau (float vitesse, int sens ) {

	if (sens) { 	//peut-être dans l'autre sens
		GPIOA -> ODR |= 0x00 ; 
	}
	else
	{
		GPIOA -> ODR |= 0x01 ;
	}
	
	vitesse=abso(vitesse); // on recoit une vitesse entre -1 et 1 et on fait un seuil entre -0.2 et 0.2 où c'est stable
	if (vitesse<0.2) {
		vitesse = 0;
	}
	else {
		 vitesse=(vitesse-0.2)/0.8 ;
	}
	
	LL_TIM_OC_SetCompareCH1(TIM2, vitesse*65454); //vitesse à convertir en pourcentage     Arr=65454
	
}