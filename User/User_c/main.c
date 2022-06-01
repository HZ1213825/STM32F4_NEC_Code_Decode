#include "stm32f4xx.h" // ע���������ļ���#define STM32F401xxΪ�Լ���оƬ�ͺ�
#include "Delay.h"
#include "Usart.h"
#include "IR_NEC.h"

int main()
{
	// u8 DAT[5] = {0X10, 0X68, 0X80, 0X03, 0X0};
	IR_NEC_Read_Init();
	// IR_NEC_Send_Init();
	Usart_init();
	// printf("12");
	// IR_NEC_Send_Code(DAT, 5);
	while (1)
	{
		if (IR_NEC_Read_OK)
		{
			for (int i = 0; i < N; i++)
				printf("0x%2X ", IR_NEC_Read_Dat[i]);
			printf("\r\n");
			IR_NEC_Read_OK = 0;
		}
	}
}
