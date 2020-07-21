#include "dma.h"
#include "delay.h"

/*
DMAy_Streamx：DMA数据流，DMA1_Stream0~7、DMA2_Stream0~7
DMA_Channel_x：DMA通道选择，DMA_Channel_0~7
Periph_Baseaddr：外设地址
Memory_Baseaddr：存储器地址
BufferSize：数据传输量
*/
void DMA_Init_Setting(DMA_Stream_TypeDef *DMAy_Streamx, u32 DMA_Channel_x, u32 Periph_Baseaddr, u32 Memory_Baseaddr, u16 BufferSize)
{

    DMA_InitTypeDef DMA_InitStructure;

    if ((u32)DMAy_Streamx > (u32)DMA2)                       //得到当前stream是属于DMA2还是DMA1
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); //DMA2时钟使能
    else
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); //DMA1时钟使能
    DMA_DeInit(DMAy_Streamx);
    while (DMA_GetCmdStatus(DMAy_Streamx) != DISABLE) //等待DMA可配置
        ;

    /*DMA Stream 配置 */
    DMA_InitStructure.DMA_Channel = DMA_Channel_x;                          //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = Periph_Baseaddr;             //DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = Memory_Baseaddr;                //DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 //存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = BufferSize;                          //数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   //中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         //存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //外设突发单次传输
    DMA_Init(DMAy_Streamx, &DMA_InitStructure);                         //初始化DMA Stream
}

/*
开启一次DMA传输
DMAy_Streamx：DMA数据流，DMA1_Stream0~7、DMA2_Stream0~7
BufferSize：数据传输量
*/
void DMA_Enable_Once(DMA_Stream_TypeDef *DMAy_Streamx, u16 BufferSize)
{

    DMA_Cmd(DMAy_Streamx, DISABLE); //关闭DMA传输

    while (DMA_GetCmdStatus(DMAy_Streamx) != DISABLE) //确保DMA可以被设置
        ;

    DMA_SetCurrDataCounter(DMAy_Streamx, BufferSize); //数据传输量

    DMA_Cmd(DMAy_Streamx, ENABLE); //开启DMA传输
}
