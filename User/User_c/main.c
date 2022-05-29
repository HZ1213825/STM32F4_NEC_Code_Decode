#include "stm32f4xx.h" // 注意更改这个文件的#define STM32F401xx为自己的芯片型号
#include "Delay.h"
#include "Usart.h"
#include "IR_NEC.h"

int main()
{
	u8 DAT[5] = {0X10, 0X68, 0X80, 0X03, 0X0};
	IR_NEC_Read_Init();
	IR_NEC_Send_Init();
	Usart_init();
	printf("12");
	IR_NEC_Send_Code(DAT, 5);
	while (1)
	{
		// if (IR_NEC_Read_OK)
		// {
		// 	for (int i = 0; i < N; i++)
		// 		printf("0x%2X ", Dat[i]);
		// 	printf("\r\n");
		// 	IR_NEC_Read_OK = 0;
		// }
	}
}
