#include "usart.h"
#include "string.h"

struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; //循环发送,直到发送完毕
	USART1->DR = (u8)ch;
	return ch;
}

/*串口USART1初始化*/
void usart1_init(uint32_t BaudRate)
{
	/*GPIO口配置*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/*UASRT配置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);

	/*中断配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);

	/*串口使能*/
	USART_Cmd(USART1, ENABLE);
}

/*串口USART1发送数据函数*/
void USART_SendDatas(USART_TypeDef *USARTx, uint8_t *SendBuf, uint8_t n)
{
	int k;
	for (k = 0; k < n; k++)
	{
		USART_SendData(USARTx, SendBuf[k]);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;
	}
}

u8 ReceiveFlag, ReceiveClear = 1, ReceiveSize = 0, ReceiveBuff[100];
/*串口USART1中断服务子程序*/
void USART1_IRQHandler(void)
{
	u8 ch;
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
	{
		ch = USART_ReceiveData(USART1);
		if (ReceiveFlag != 2)
		{
			if (ReceiveFlag == 1)
			{
				if (ch == 0x0a) //接收到‘\n’，接收结束
				{
					ReceiveFlag = 2;
					ReceiveClear = 0;
				}
				else //结束错误没完成
				{
					ReceiveFlag = 0;
				}
			}
			else
			{
				if (ch == 0x0d) //接收到‘\r’
				{
					ReceiveFlag = 1;
				}
				else
				{
					if (ReceiveClear == 0)
					{
						ReceiveSize = 0;
						memset(ReceiveBuff, 0, 100);
						ReceiveClear = 1;
					}
					ReceiveBuff[ReceiveSize] = ch;
					ReceiveSize++;
					if (ReceiveSize > 100 - 1)
						ReceiveFlag = 0;
				}
			}
		}

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
