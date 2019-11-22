//HORLOGE GENERALE

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"

#include "bordage.h"

void (*Ptr_ItFct_TIM4)(void);

//////////////////////////////////////////////////////////////////////////
int girouette;

void run_IT(void){
	
	girouette++;



}

void run(void){ //a mettre dans le while du main
	
	if (girouette==5){
		get_angle();
		girouette=0;
	}
	
	
}



//////////////////////////////////////////////////////////////////////////
void funITinit(void (*IT_function)(void)){
	Ptr_ItFct_TIM4=IT_function;
}
	
void timer_HG_init(){
	
	//init timer
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	
	LL_TIM_InitTypeDef timer_init;
	timer_init.Autoreload=999;
	timer_init.Prescaler=719;
	timer_init.ClockDivision=LL_TIM_CLOCKDIVISION_DIV1;
	timer_init.CounterMode=LL_TIM_COUNTERMODE_UP;
	timer_init.RepetitionCounter=0;
	
	LL_TIM_Init(TIM4,&timer_init);

	//init IT
	funITinit(run_IT);
	// validation du canal NVIC
	IRQn_Type TIM_irq;
	
	TIM_irq=TIM4_IRQn;
	
	NVIC_SetPriority(TIM_irq, 3);
	NVIC_EnableIRQ(TIM_irq);
	
	LL_TIM_EnableIT_UPDATE(TIM4);
	
	LL_TIM_EnableCounter(TIM4);
	
	girouette=0;
}

void TIM4_IRQHandler(void)
{
	// rabaisser le flag d'IT
	LL_TIM_ClearFlag_UPDATE(TIM4);
	(*Ptr_ItFct_TIM4)();
}	