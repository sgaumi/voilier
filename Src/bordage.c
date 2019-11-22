
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"

#include "bordage.h"



//girouette

static float angle = 0. ;	//en degr�

void gpio_gigi_init(){
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	
	//signal A sur PA0 (TIM2_CH1)
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_FLOATING);
	
	//signal B sur PA1 (TIM2_CH2)
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_FLOATING);
	
	//index sur PA2
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_FLOATING);
	

}

void timer_gigi_init(){
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
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
	
	LL_TIM_ENCODER_Init(TIM2, &tim_init); //initialisation encoder mode
	
	LL_TIM_EnableIT_UPDATE(TIM2);	//autorise les interruptions
	
	LL_TIM_EnableCounter(TIM2);	//activation
	

}


void get_angle(){
	
	int val;
	
	if ( LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_2)){	//passage a 0
			LL_TIM_SetCounter(TIM2, 0x0000);
			angle = 0.;
		
	}
	else{
		val = LL_TIM_GetCounter(TIM2);
		angle = ((val%720)/2);	//calcul angle
	}
	

}



//sevo-moteur


//moteur-girouette



//batterie?