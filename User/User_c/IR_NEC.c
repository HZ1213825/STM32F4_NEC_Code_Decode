#include "IR_NEC.h"
u8 IR_NEC_Read_ins = 0;      //指示解码类型 引导码/数据码
u8 IR_NEC_Read_Decode_i = 0; //解码循环变量
u8 IR_NEC_Read_Decode_j = 0; //解码循环变量
u8 IR_NEC_Read_OK = 0;       //解码成功标志
u32 IR_NEC_Read_Time = 0;    //下降沿时间差
u8 IR_NEC_Read_zj = 0;
u8 IR_NEC_Send_Square = 0; //方波
u8 IR_NEC_Read_Dat[N] = {0};
u8 IR_NEC_Read_Dat2[N] = {0};
void air(void)
{
}
// 读取，GPIO初始化
void IR_NEC_Read_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_Initstruct;                //声明GPIO初始化结构体
    IR_NEC_Read_GPIO_RCC;                            //打开GPIO时钟
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN;        //输入模式
    GPIO_Initstruct.GPIO_OType = GPIO_OType_OD;      //开漏输入模式
    GPIO_Initstruct.GPIO_Pin = IR_NEC_Read_GPIO_Pin; //引脚0
    GPIO_Initstruct.GPIO_PuPd = GPIO_PuPd_UP;        //上拉模式
    GPIO_Initstruct.GPIO_Speed = GPIO_High_Speed;    //高速模式
    GPIO_Init(IR_NEC_Read_GPIOx, &GPIO_Initstruct);  //初始化GPIO
}
//读取，外部中断初始化
void IR_NEC_Read_EXTI_init(void)
{
    EXTI_InitTypeDef EXTI_Initstruct;                      //创建外部中断初始化结构体
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); //打开时钟
    IR_NEC_Read_EXIT_Link;                                 //将GPIO与外部中断连接
    EXTI_Initstruct.EXTI_Line = IR_NEC_Read_EXIT_Pin;      //配置的是外部中断0
    EXTI_Initstruct.EXTI_LineCmd = ENABLE;                 //使能
    EXTI_Initstruct.EXTI_Mode = EXTI_Mode_Interrupt;       //选择中断模式
    EXTI_Initstruct.EXTI_Trigger = EXTI_Trigger_Falling;   //下降沿模式
    EXTI_Init(&EXTI_Initstruct);                           //初始化外部中断0
}
//读取，配置NVIC
void IR_NEC_Read_EXTI_NVIC(void)
{
    NVIC_InitTypeDef NVIC_Initstruct;                                                //声明NVIC初始化结构体
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                  //选定NVIC分组
    NVIC_Initstruct.NVIC_IRQChannel = IR_NEC_Read_EXIT_IRQn;                         //配置的外部中断0
    NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;                                     //使能
    NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = IR_NEC_Read_EXIT_Priority_1; //主优先级
    NVIC_Initstruct.NVIC_IRQChannelSubPriority = IR_NEC_Read_EXIT_Priority_2;        //副优先级
    NVIC_Init(&NVIC_Initstruct);                                                     //初始化外部中断0的NVIC
}
//读取，定时器初始化
void IR_NEC_Read_TIM_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_Init_Struct;              //声明定时器初始化结构体
    NVIC_InitTypeDef NVIC_Init_Struct;                    //声明NVIC初始化结构体
    IR_NEC_Read_TIM_RCC;                                  //打开时钟
    TIM_Init_Struct.TIM_ClockDivision = TIM_CKD_DIV1;     //滤波器不分频
    TIM_Init_Struct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    //每次中断触发时间=[(TIM_Period+1)*(TIM_Prescaler+1)/(SystemCoreClock)] (s)
    TIM_Init_Struct.TIM_Prescaler = 84 - 1;
    TIM_Init_Struct.TIM_Period = 0xffff - 1;
    TIM_Init_Struct.TIM_RepetitionCounter = 0;                 //高级定时器特有，这里写0就行
    TIM_TimeBaseInit(IR_NEC_Read_TIM_TIMx, &TIM_Init_Struct);  //调用函数初始
    TIM_ITConfig(IR_NEC_Read_TIM_TIMx, TIM_IT_Update, ENABLE); //启用溢出中断

    NVIC_Init_Struct.NVIC_IRQChannel = IR_NEC_Read_TIM_IRQn;                         //中断名称
    NVIC_Init_Struct.NVIC_IRQChannelCmd = ENABLE;                                    //使能
    NVIC_Init_Struct.NVIC_IRQChannelPreemptionPriority = IR_NEC_Read_TIM_Priority_1; //主优先级1
    NVIC_Init_Struct.NVIC_IRQChannelSubPriority = IR_NEC_Read_TIM_Priority_2;        //副优先级1
    NVIC_Init(&NVIC_Init_Struct);                                                    //初始化NVIC
    TIM_Cmd(IR_NEC_Read_TIM_TIMx, ENABLE);                                           //打开定时器
}
//读取初始化函数
void IR_NEC_Read_Init(void)
{
    IR_NEC_Read_GPIO_init();
    IR_NEC_Read_EXTI_init();
    IR_NEC_Read_EXTI_NVIC();
    IR_NEC_Read_TIM_init();
}

// NEC解码函数，外部中断下降沿调用
void IR_NEC_Read_Decode(void (*val)(void))
{
    if (IR_NEC_Read_ins == 0) //检测初始低电平
    {
        IR_NEC_Read_ins = 1;
        TIM_SetCounter(IR_NEC_Read_TIM_TIMx, 0);
    }
    else if (IR_NEC_Read_ins == 1) //判断初始低电平到第二个低电平时间
    {
        IR_NEC_Read_Time = TIM_GetCounter(IR_NEC_Read_TIM_TIMx);
        if (IR_NEC_Read_Time > 13500 - 500 && IR_NEC_Read_Time < 13500 + 500) // 13.5ms左右 引导码
        {
            IR_NEC_Read_ins = 2;
            TIM_SetCounter(IR_NEC_Read_TIM_TIMx, 0);
            IR_NEC_Read_OK = 0;
            for (int i = 0; i < N; i++)
                IR_NEC_Read_Dat[i] = 0;
            IR_NEC_Read_zj = 0;
        }
        else if (IR_NEC_Read_Time > 11250 - 1000 && IR_NEC_Read_Time < 11250 + 1000) // 11.25ms左右 重复码
        {
            IR_NEC_Read_ins = 0;
            IR_NEC_Read_OK = 2;
            for (int i = 0; i < N; i++)
                IR_NEC_Read_Dat[i] = IR_NEC_Read_Dat2[i];

            TIM_SetCounter(IR_NEC_Read_TIM_TIMx, 0);
        }
        else //超时或时间过短 复位
        {
            IR_NEC_Read_ins = 0;
        }
    }
    else if (IR_NEC_Read_ins == 2) //开始解码
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        IR_NEC_Read_Time = TIM_GetCounter(IR_NEC_Read_TIM_TIMx);
        if (IR_NEC_Read_Time > 1120 - 500 && IR_NEC_Read_Time < 1120 + 500) // 1.12ms 写入0
        {
            IR_NEC_Read_zj <<= 1;   //向左移位
            IR_NEC_Read_zj &= 0xfe; //最低位置零
            IR_NEC_Read_Decode_i++;
        }
        else if (IR_NEC_Read_Time > 2250 - 500 && IR_NEC_Read_Time < 2250 + 500) // 2.25ms 写入1
        {
            IR_NEC_Read_zj <<= 1;
            IR_NEC_Read_zj |= 0x01;
            IR_NEC_Read_Decode_i++;
        }
        else //出错复位
        {
            IR_NEC_Read_ins = 0;
            IR_NEC_Read_Decode_i = 0;
            IR_NEC_Read_Decode_j = 0;
            IR_NEC_Read_zj = 0;
        }

        if (IR_NEC_Read_Decode_i >= 8) // uchar每位写入数据
        {
            IR_NEC_Read_Decode_i = 0;
            IR_NEC_Read_Dat[IR_NEC_Read_Decode_j] = IR_NEC_Read_zj;
            IR_NEC_Read_Decode_j++;
            IR_NEC_Read_zj = 0;
        }
        if (IR_NEC_Read_Decode_j >= N) //数据数组的不同位写入数据
        {
            IR_NEC_Read_Decode_i = 0;
            IR_NEC_Read_Decode_j = 0;
            IR_NEC_Read_ins = 0;
            IR_NEC_Read_OK = 1;
            IR_NEC_Read_zj = 0;
            for (int i = 0; i < N; i++)
                IR_NEC_Read_Dat2[i] = IR_NEC_Read_Dat[i];
            val();
        }

        TIM_SetCounter(IR_NEC_Read_TIM_TIMx, 0);
    }
}

// 发送，GPIO初始化
void IR_NEC_Send_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_Initstruct;                //声明GPIO初始化结构体
    IR_NEC_Send_GPIO_RCC;                            //打开GPIO时钟
    GPIO_Initstruct.GPIO_Mode = GPIO_Mode_OUT;       //输出模式
    GPIO_Initstruct.GPIO_OType = GPIO_OType_PP;      //推挽输出模式
    GPIO_Initstruct.GPIO_Pin = IR_NEC_Send_GPIO_Pin; //引脚0
    GPIO_Initstruct.GPIO_PuPd = GPIO_PuPd_UP;        //上拉模式
    GPIO_Initstruct.GPIO_Speed = GPIO_High_Speed;    //高速模式
    GPIO_Init(IR_NEC_Send_GPIOx, &GPIO_Initstruct);  //初始化GPIO
}
//发送，定时器初始化
void IR_NEC_Send_TIM_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_Init_Struct;              //声明定时器初始化结构体
    NVIC_InitTypeDef NVIC_Init_Struct;                    //声明NVIC初始化结构体
    IR_NEC_Send_TIM_RCC;                                  //打开时钟
    TIM_Init_Struct.TIM_ClockDivision = TIM_CKD_DIV1;     //滤波器不分频
    TIM_Init_Struct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    //每次中断触发时间=[(TIM_Period+1)*(TIM_Prescaler+1)/(SystemCoreClock)] (s) //13。15us
    TIM_Init_Struct.TIM_Prescaler = 221 - 1;                                         // 221
    TIM_Init_Struct.TIM_Period = 5 - 1;                                              // 5
    TIM_Init_Struct.TIM_RepetitionCounter = 0;                                       //高级定时器特有，这里写0就行
    TIM_TimeBaseInit(IR_NEC_Send_TIM_TIMx, &TIM_Init_Struct);                        //调用函数初始
    TIM_ITConfig(IR_NEC_Send_TIM_TIMx, TIM_IT_Update, ENABLE);                       //启用溢出中断
                                                                                     //
    NVIC_Init_Struct.NVIC_IRQChannel = IR_NEC_Send_TIM_IRQn;                         //中断名称
    NVIC_Init_Struct.NVIC_IRQChannelCmd = ENABLE;                                    //使能
    NVIC_Init_Struct.NVIC_IRQChannelPreemptionPriority = IR_NEC_Send_TIM_Priority_1; //主优先级
    NVIC_Init_Struct.NVIC_IRQChannelSubPriority = IR_NEC_Send_TIM_Priority_2;        //副优先级
    NVIC_Init(&NVIC_Init_Struct);                                                    //初始化NVIC
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
}

//发送初始化函数
void IR_NEC_Send_Init(void)
{
    IR_NEC_Send_TIM_init();
    IR_NEC_Send_GPIO_init();
}
//发送引导码
__STATIC_INLINE void IR_NEC_Send_Guide(void)
{
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    TIM_SetCounter(IR_NEC_Send_TIM_TIMx, 0);
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, ENABLE);
    Delay_us(9000); // 9ms闪
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    Delay_us(4500); // 4.5ms灭
    //共13.5ms
}
//发送数据0
__STATIC_INLINE void IR_NEC_Send_0(void)
{
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    TIM_SetCounter(IR_NEC_Send_TIM_TIMx, 0);
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, ENABLE);
    Delay_us(620); // 0.56ms闪
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    Delay_us(500); // 0.56ms灭
    //共1.12ms
}
//发送数据1
__STATIC_INLINE void IR_NEC_Send_1(void)
{
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    TIM_SetCounter(IR_NEC_Send_TIM_TIMx, 0);
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, ENABLE);
    Delay_us(625); // 0.56ms闪
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    Delay_us(1600); // 1.69ms灭
    //共2.25ms
}
void IR_NEC_Send_End(void)
{
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, ENABLE);
    Delay_us(650); // 6ms亮
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
}
//发送重复码
void IR_NEC_Send_Repect(void)
{
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    TIM_SetCounter(IR_NEC_Send_TIM_TIMx, 0);
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, ENABLE);
    Delay_us(9000); // 9ms亮
    TIM_Cmd(IR_NEC_Send_TIM_TIMx, DISABLE);
    GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
    Delay_us(2250); // 2.25ms灭
                    //共11.25ms
    IR_NEC_Send_End();
    Delay_ms(100);
}
void IR_NEC_Send_Code(u8 *Dat, u32 Len)
{
    u32 zj;
    IR_NEC_Send_Guide();
    for (int j = 0; j < Len; j++)
    {
        zj = Dat[j];
        for (int i = 0; i < 8; i++)
        {
            if (zj & (0X80))
            {
                IR_NEC_Send_1();
            }
            else
            {
                IR_NEC_Send_0();
            }
            zj <<= 1;
        }
    }
    IR_NEC_Send_End();
    Delay_ms(40);
}
//产生38kHz方波
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(IR_NEC_Send_TIM_TIMx, TIM_IT_Update) != RESET)
    {
        if (IR_NEC_Send_Square)
            GPIO_SetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
        else
            GPIO_ResetBits(IR_NEC_Send_GPIOx, IR_NEC_Send_GPIO_Pin);
        IR_NEC_Send_Square++;
        if (IR_NEC_Send_Square >= 2)
            IR_NEC_Send_Square = 0;

        TIM_ClearITPendingBit(IR_NEC_Send_TIM_TIMx, TIM_IT_Update); //将中断标志清除
    }
}
//定时器中断，不能没有
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(IR_NEC_Read_TIM_TIMx, TIM_IT_Update) != RESET)
    {

        TIM_ClearITPendingBit(IR_NEC_Read_TIM_TIMx, TIM_IT_Update); //将中断标志清除
    }
}
//外部中断0的中断服务函数
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(IR_NEC_Read_EXIT_Pin) != RESET) //标志位被值位（产生中断）
    {
        IR_NEC_Read_Decode(air);
        EXTI_ClearITPendingBit(IR_NEC_Read_EXIT_Pin); //清除中断标志位
    }
}
