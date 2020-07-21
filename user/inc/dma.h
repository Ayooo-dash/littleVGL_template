#ifndef __DMA_H__
#define __DMA_H__
#include "stm32f4xx.h"

void DMA_Init_Setting(DMA_Stream_TypeDef *DMAy_Streamx, u32 DMA_Channel_x, u32 Periph_Baseaddr, u32 Memory_Baseaddr, u16 BufferSize);
void DMA_Enable_Once(DMA_Stream_TypeDef *DMAy_Streamx, u16 BufferSize);

#endif
