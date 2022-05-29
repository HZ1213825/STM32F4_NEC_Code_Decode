#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f4xx.h"

extern uint32_t Delay_time;
void Delay_us(uint32_t time);
void SysTick_Handler(void);
void Delay_ms(uint32_t time);
void Delay_s(uint32_t time);
void Delay_min(uint32_t time);


#endif
