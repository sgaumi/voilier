//HORLOGE GENERALE

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"

#include "bordage.h"


void run_IT(void);

void run(void);

/*void funITinit(void (*IT_function)(void));
	
void timer_HG_init();

void TIM4_IRQHandler(void);*/



void funITinit_st(void);

void timer_HG_init_st(void);

void SysTick_IRQHandler(void);