#ifndef _USART_H
#define _USART_H
#include <stdio.h>
#include "stm32f4xx.h"
// GPIO初始化相关
#define Usart_GPIOx GPIOA                  // GPIO组号
#define Usart_TXD GPIO_Pin_9               //发送脚，需要和串口号配合，请查文档
#define Usart_RXD GPIO_Pin_10              //接收脚
#define Usart_GPIORCC RCC_AHB1Periph_GPIOA // GPIO时钟
/*设置GPIO复用相关*/
#define Usart_GPIO_AF_Source_TXD GPIO_PinSource9  //选择发送脚
#define Usart_GPIO_AF_Source_RXD GPIO_PinSource10 //选择接收脚
#define Usart_GPIO_AF_Aim GPIO_AF_USART1          //选择硬件串口号
/*硬件串口初始化相关*/
#define Usart_USARTx USART1                                                  //硬件串口号
#define Usart_Clockcmd RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE) //串口时钟
#define Usart_BaudRate 9600                                                  //波特率
#define Usart_Parity USART_Parity_No                                         //校验位
#define Usart_StopBits USART_StopBits_1                                      //停止位
#define Usart_WordLength USART_WordLength_8b                                 //数据长度
#define Usart_IRQEN 0                                                        //中断使能（使用scanf关闭（0），使用中断打开（1））
#define Usart_IRQHandler USART1_IRQHandler                                   //中断服务函数
/*NVIC相关*/
#define Usart_IRQChannel USART1_IRQn         //中断名
#define Usart_IRQChannelPreemptionPriority 2 //主优先级
#define Usart_IRQChannelSubPriority 2        //副优先级
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void Usart_init(void);
void Usart_GPIO_init(void);
void Usart_NVIC_init(void);
void Usart_IRQHandler(void);
#endif
