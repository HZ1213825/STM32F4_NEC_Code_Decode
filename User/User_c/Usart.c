#include "Usart.h"
u8 Ctl[6] = {0};
u8 i = 0;
u8 Usart_OK = 0;
u32 dat = 0;
//重定向，定向到串口输出
int fputc(int ch, FILE *f)
{

    USART_SendData(Usart_USARTx, (uint8_t)ch); //发送数据

    while (USART_GetFlagStatus(Usart_USARTx, USART_FLAG_TXE) == RESET) //等待发送完成
        ;

    return (ch);
}

//重定向，定向到串口输入
int fgetc(FILE *f)
{

    while (USART_GetFlagStatus(Usart_USARTx, USART_FLAG_RXNE) == RESET) //等待接收到数据
        ;

    return (int)USART_ReceiveData(Usart_USARTx); //返回数据
}
void Usart_NVIC_init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStruct.NVIC_IRQChannel = Usart_IRQChannel;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = Usart_IRQChannelPreemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = Usart_IRQChannelSubPriority;
    NVIC_Init(&NVIC_InitStruct);
}
void Usart_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(Usart_GPIORCC, ENABLE);
    GPIO_PinAFConfig(Usart_GPIOx, Usart_GPIO_AF_Source_TXD, Usart_GPIO_AF_Aim);
    GPIO_PinAFConfig(Usart_GPIOx, Usart_GPIO_AF_Source_RXD, Usart_GPIO_AF_Aim);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = Usart_TXD | Usart_RXD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(Usart_GPIOx, &GPIO_InitStruct);
}
void Usart_init(void)
{
    USART_InitTypeDef USART_InitStruct;
    Usart_GPIO_init();
    Usart_NVIC_init();

    Usart_Clockcmd;
    USART_InitStruct.USART_BaudRate = Usart_BaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(Usart_USARTx, &USART_InitStruct);
    if (Usart_IRQEN)
    {
        USART_ITConfig(Usart_USARTx, USART_IT_RXNE, ENABLE); //使用中断接收时使用这句，使用scanf时关闭这句
    }
    USART_Cmd(Usart_USARTx, ENABLE);
}
void Usart_IRQHandler(void)
{
    u8 zj = 0;
    if (USART_GetITStatus(Usart_USARTx, USART_IT_RXNE) != RESET)
    {
        zj = USART_ReceiveData(Usart_USARTx);
        if (Usart_OK == 0)
        {
            Ctl[i] = zj;
            i++;
            if (i > 4)
                i = 0;
            if (Ctl[0] == 's' && Ctl[1] == 't' && Ctl[2] == 'a' && Ctl[3] == 'r' && Ctl[4] == 't')
            {
                Usart_OK = 1;
                for (int j = 0; j < 5; j++)
                    Ctl[j] = '\0';
                i = 0;
                printf("dat:\r\n");
                dat = 0;
            }
        }
        else if (Usart_OK == 1)
        {
            i++;
            dat |= zj;
            if (i == 4)
            {
                Usart_OK = 2;
                i = 0;
            }
            else
            {
                dat <<= 8;
            }
        }

        USART_ClearITPendingBit(Usart_USARTx, USART_IT_RXNE);
    }
}
