#include "stm32f10x.h"
#include "delay.h"

void delayInit() {
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
	TIM15->PSC = (24000000 / 1000000) - 1;
	TIM15->CR1 = TIM_CR1_OPM;
	TIM15->EGR = TIM_EGR_UG;
}

void delayUs(int us) {
	TIM15->ARR = us;
	TIM15->CR1 |= TIM_CR1_CEN;
	while((TIM15->CR1 & TIM_CR1_CEN) != 0);
}

void delayMs(int ms) {
	while(ms--)
		delayUs(1000);
}
