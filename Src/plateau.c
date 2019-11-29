//plateau.c

/*
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
	
	



}



*/