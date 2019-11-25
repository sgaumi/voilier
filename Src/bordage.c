
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"

#include "bordage.h"



//girouette

static float angle = 0. ;	//en degré

void gpio_gigi_init(){
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	
	//signal A sur PA6 (TIM3_CH1)
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_FLOATING);
	
	//signal B sur PA7 (TIM3_CH2)
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_FLOATING);
	
	//index sur PA5
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_FLOATING);
	

}

void timer_gigi_init(){
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	
	LL_TIM_ENCODER_InitTypeDef tim_init;
	tim_init.EncoderMode = LL_TIM_ENCODERMODE_X4_TI12;
	tim_init.IC1Polarity = LL_TIM_IC_POLARITY_RISING; //pas sur?
	tim_init.IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	tim_init.IC1Prescaler = LL_TIM_ICPSC_DIV1;
	tim_init.IC1Filter = LL_TIM_IC_FILTER_FDIV1; 
	tim_init.IC2Polarity = LL_TIM_IC_POLARITY_RISING;	//pas sur?
	tim_init.IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	tim_init.IC2Prescaler = LL_TIM_ICPSC_DIV1;
	tim_init.IC2Filter = LL_TIM_IC_FILTER_FDIV1;
	
	LL_TIM_ENCODER_Init(TIM3, &tim_init); //initialisation encoder mode
	
	LL_TIM_EnableIT_UPDATE(TIM3);	//autorise les interruptions
	
	LL_TIM_EnableCounter(TIM3);	//activation
	

}


void get_angle(){
	
	int val;
	
	if ( LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_5)){	//passage a 0
			LL_TIM_SetCounter(TIM3, 0x8000); //32768
			angle = 0.;
		
	}
	else{
		val = LL_TIM_GetCounter(TIM3);
		angle = (((float)((val-32768)%1440))/4.);		//calcul angle
		if (angle>180.){
			angle=angle-360;
		}
		else if(angle<(-180.)){
			angle=angle+360;
		}
	}
	

}



//sevo-moteur

float abso(float a){
	if (a<0){
		a=-a;
	}
	return(a);
}

void generatePWM (){
	
//faire une pwm envoyé au servo-moteur

//utiliser timer1 sur PA8 pour faire pwm
//mettre PA8 en output car de là sort la pwm initialisée en timer CH1 
//CCR1 en fonction de alpha
	float alpha2; 
	//on reçoit alpha entre -180 et 180 donc on prend la valeur absolue
	if (abso(angle)< 45) { 
		alpha2 = 0. ; }
	else { //angle entre 45 et 180
		alpha2 = (abso(angle)-45.)/135. *90 ; //angle du moteur entre 0 et 90
	}
	
	float beta = (alpha2/180.)*100 ; //passe l'angle en pourcentage
	
	int CCR1Max = 6545; //ARR*0,1
	int CCR1Min = 3272;  //ARR*0,05
	LL_TIM_OC_SetCompareCH1(TIM1, beta*(CCR1Max-CCR1Min)+CCR1Min);
		
	
}

void timer_pwm_init(){
	int Psc =21;
	int Arr = 65454; 
	LL_TIM_InitTypeDef Timer;
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	
	Timer.Autoreload= Arr ;
	Timer.Prescaler= Psc;
	Timer.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	Timer.CounterMode=LL_TIM_COUNTERMODE_UP;
	Timer.RepetitionCounter=0;
	
	LL_TIM_Init(TIM1,&Timer); //TIM1
	
	LL_TIM_OC_SetMode (TIM1, LL_TIM_CHANNEL_CH1,LL_TIM_OCMODE_PWM1);
	TIM1->BDTR |= TIM_BDTR_MOE; //uniquement pour TIM 1 et 8
	
	TIM1->CCER |= TIM_CCER_CC1E;
	
	
	LL_TIM_EnableCounter(TIM1);

}


void gpio_servom_init() {

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	LL_GPIO_InitTypeDef pa8;
	pa8.Pin = LL_GPIO_PIN_8;  //PA8
	pa8.Mode = LL_GPIO_MODE_ALTERNATE;
	pa8.Speed = LL_GPIO_MODE_OUTPUT_10MHz;
	pa8.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
	pa8.Pull = LL_GPIO_PULL_UP ;

	LL_GPIO_Init(GPIOA, &pa8);
}

