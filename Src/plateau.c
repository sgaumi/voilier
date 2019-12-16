//plateau.c
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges
#include "stm32f1xx_ll_tim.h"  // Pour le timer

#include "plateau.h"
#include "bordage.h" //pour abso

float vitesse;
int sens;

void gpio_RF_init(){

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	//PB6
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_FLOATING);  
	//PB7
	//LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_FLOATING);


}

void timer_RF_init(){

	//init timer
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	
	LL_TIM_InitTypeDef timer_init;
	timer_init.Autoreload=19999; 
	timer_init.Prescaler=71; //pour avoir 1 000 000 Tick/s (72 000 000/(psc+1))
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
	
	LL_TIM_EnableCounter(TIM4);	
	
	vitesse=0.;
	sens=0;
	
}

void get_vitesse_sens(){
	int pulse = LL_TIM_OC_GetCompareCH2(TIM4) +1;
	int period = LL_TIM_OC_GetCompareCH1(TIM4) +1;
	
	pulse=pulse;
	vitesse = ((float)(pulse)/1000.)*2. -3.; //vitesse entre -1 et 1
	//vitesse=-0.;	
	if (vitesse>=0.){
		sens=1;
	}
	else{
		sens=0;
	}
	
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void gpio_mcc_init() {
	
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);  //GPIOA sur APB2
	//configuration de la PA1 pour générer la PWM
	//pwm PA1 tim 2 ch2
	LL_GPIO_InitTypeDef pa1;
	pa1.Pin = LL_GPIO_PIN_1;
	pa1.Mode = LL_GPIO_MODE_ALTERNATE;
	pa1.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa1.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa1.Pull = LL_GPIO_PULL_UP ;
	
	LL_GPIO_Init(GPIOA, &pa1);
	
	//configuration de la PA2 qui gère le sens (en 0 ou 1)
	//sens PA2 tim 2 ch3
	LL_GPIO_InitTypeDef pa2;
	pa2.Pin = LL_GPIO_PIN_2;
	pa2.Mode = LL_GPIO_MODE_OUTPUT;
	pa2.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa2.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa2.Pull = LL_GPIO_PULL_UP ;
	
	LL_GPIO_Init(GPIOA, &pa2);


	
}		
	
void timer_pwm_mcc_init() {
	
	int Arr = 65454 ;
	int Psc = 54 ;

	LL_TIM_InitTypeDef Timer;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	//timer correspondant à la PWM (TIM2)
	Timer.Autoreload= Arr;
	Timer.Prescaler= Psc;
	Timer.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	Timer.CounterMode=LL_TIM_COUNTERMODE_UP;
	Timer.RepetitionCounter=0;
		
	LL_TIM_Init(TIM2,&Timer);
	
	LL_TIM_OC_SetMode (TIM2, LL_TIM_CHANNEL_CH2,LL_TIM_OCMODE_PWM1);


	TIM2->CCER |= TIM_CCER_CC2E;
	
	LL_TIM_EnableCounter(TIM2);
	
	
}	
	
int i=0;
	
void bougerPlateau() {
	
	float vitesse2;
	int vit2;

	if (sens) { 	//peut-être dans l'autre sens
		GPIOA -> ODR &= ~0x00000004 ; 
	}
	else
	{
		GPIOA -> ODR |= 0x00000004 ;
	}
	
	vitesse2=abso(vitesse); // on recoit une vitesse entre -1 et 1 et on fait un seuil entre -0.2 et 0.2 où c'est stable
	if (vitesse2<0.2) {
		vitesse2 = 0.0;
	}
	else {
		 vitesse2=((vitesse2-0.2)/0.8)*65454.0 ;
	}
	vit2 = (int)(vitesse2) ;
	LL_TIM_OC_SetCompareCH2(TIM2, vit2); //vitesse à convertir en pourcentage     Arr=65454 //2339162.5
}