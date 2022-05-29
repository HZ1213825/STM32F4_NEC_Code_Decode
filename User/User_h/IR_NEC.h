#ifndef _IR_NEC_H_
#define _IR_NEC_H_
#include "stm32f4xx.h" // 注意更改这个文件的#define STM32F401xx为自己的芯片型号
#include "Delay.h"
//读取部分
// GPIO
#define IR_NEC_Read_GPIO_RCC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define IR_NEC_Read_GPIOx GPIOB
#define IR_NEC_Read_GPIO_Pin GPIO_Pin_0

//外部中断
#define IR_NEC_Read_EXIT_Link SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0)
#define IR_NEC_Read_EXIT_Pin EXTI_Line0
#define IR_NEC_Read_EXIT_IRQn EXTI0_IRQn
#define IR_NEC_Read_EXIT_Priority_1 1
#define IR_NEC_Read_EXIT_Priority_2 1

//定时器
#define IR_NEC_Read_TIM_RCC RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define IR_NEC_Read_TIM_TIMx TIM3
#define IR_NEC_Read_TIM_IRQn TIM3_IRQn
#define IR_NEC_Read_TIM_Priority_1 2
#define IR_NEC_Read_TIM_Priority_2 2

//发送部分
// GPIO
#define IR_NEC_Send_GPIO_RCC RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define IR_NEC_Send_GPIOx GPIOA
#define IR_NEC_Send_GPIO_Pin GPIO_Pin_0

//定时器
#define IR_NEC_Send_TIM_RCC RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define IR_NEC_Send_TIM_TIMx TIM4
#define IR_NEC_Send_TIM_IRQn TIM4_IRQn
#define IR_NEC_Send_TIM_Priority_1 1
#define IR_NEC_Send_TIM_Priority_2 2

#define N 4
extern u8 IR_NEC_Read_Dat[N]; //解码的数据
extern u8 IR_NEC_Read_OK;     //解码成功标志

void air(void);
void IR_NEC_Read_Init(void);
void IR_NEC_Read_Decode(void (*val)(void));
void IR_NEC_Send_Init(void);
void IR_NEC_Send_Code(u8 *Dat, u32 Len);
void IR_NEC_Send_Repect(void);
#endif
