#ifndef __USART_H__
#define __USART_H__
#include "stm32f4xx.h"
#include "stdio.h"
#include "stm32f4xx_conf.h"

void usart1_init(uint32_t BaudRate);

void USART_SendDatas(USART_TypeDef * USARTx, uint8_t *SendBuf, uint8_t n);

#endif
