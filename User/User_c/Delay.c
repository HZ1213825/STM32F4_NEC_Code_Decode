#include"Delay.h"


uint32_t Delay_time=0;
void Delay_us(uint32_t time)
{
	SysTick_Config((SystemCoreClock/1000000)-1);
	NVIC_SetPriority(SysTick_IRQn,0);
	Delay_time=time;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;//打开SysTick定时器
	while(Delay_time);
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭SysTick定时器
}

void SysTick_Handler(void)
{
	if(Delay_time)
	{
		Delay_time--;
	}
}

void Delay_ms(uint32_t time)
{
	while(time--)
	{
		Delay_us(1000);
	}
}

void Delay_s(uint32_t time)
{
	while(time--)
	{
		Delay_ms(1000);
	}
}
void Delay_min(uint32_t time)
{
	while(time--)
	{
		Delay_s(60);
	}
}
