#include "FTU.h"
#include "User.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "Lora.h"


FTU_t FTU={
            .RxLEDStatus = 0,
            .TxLEDStatus = 0,

            .TxBuff = {'0'},
            .RxBuff = {'0'},

            .RxSize = 0,
            .TxSize = 0,
            .RxCount = 0,
            .RxTime = 0,
            
            .ReadIDFlag = 0,
            .UartHander = &huart3,
};



void FTU_CopyToLora(uint8_t *Rx_Buff, uint16_t Rx_Size);


void FTU_Init(void)
{
  
  FTU.pFTU_RXBuff_In = FTU.FTU_RXBuff_Pointer;
  FTU.pFTU_RXBuff_Out = FTU.pFTU_RXBuff_In;
  FTU.pFTU_RXBuff_End = &FTU.FTU_RXBuff_Pointer[FTU_RX_PT_NUM - 1];
  FTU.pFTU_RXBuff_In ->pStart = FTU.RxBuff;
  
  FTU.RxDataCount = 0;
  
  
  
  __HAL_UART_CLEAR_IDLEFLAG(FTU.UartHander);//清除标志位
	__HAL_UART_ENABLE_IT(FTU.UartHander, UART_IT_IDLE );   
  
  

//  HAL_UART_Receive_DMA(FTU.UartHander, FTU.RxBuff, FTU_RX_MAX_SIZE);
 
  HAL_UART_Receive_DMA(FTU.UartHander, FTU.pFTU_RXBuff_In ->pStart, FTU_RX_MAX_SIZE);  
  
}

int FTU_IRQHandler(void)
{
  if(NULL == FTU.UartHander)
    return -1;
	if (__HAL_UART_GET_FLAG(FTU.UartHander, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(FTU.UartHander);//清除标志位
    HAL_UART_DMAStop(FTU.UartHander);
    
    FTU.RxSize = FTU_RX_MAX_SIZE - __HAL_DMA_GET_COUNTER(FTU.UartHander->hdmarx);// huart1.hdmarx->Instance->CNDTR;
    
		
	// HAL_UART_Transmit(User.UartHander,FTU.pFTU_RXBuff_In->pStart, FTU.RxSize,500);

    FTU.RxDataCount += FTU.RxSize;
    
    FTU.pFTU_RXBuff_In->pEnd = &FTU.RxBuff[FTU.RxDataCount];
    FTU.pFTU_RXBuff_In++;
    
    if( FTU.pFTU_RXBuff_In == FTU.pFTU_RXBuff_End)
    {
      FTU.pFTU_RXBuff_In = &FTU.FTU_RXBuff_Pointer[0];
    }
    
    
    if(FTU_RX_BUFF_MAX_SIZE - FTU.RxDataCount >= FTU_RX_MAX_SIZE)
    {
      FTU.pFTU_RXBuff_In->pStart = &FTU.RxBuff[FTU.RxDataCount];
    }
    else
    {
      FTU.RxDataCount = 0;
      FTU.pFTU_RXBuff_In ->pStart = FTU.RxBuff;
    }
    
    FTU.RxCount = FTU.RxCount > 50000 ? 1 : FTU.RxCount + 1;
    
    FTU.RxTime = 0;
    HAL_UART_Receive_DMA(FTU.UartHander, FTU.pFTU_RXBuff_In ->pStart, FTU_RX_MAX_SIZE);
    
  }
    
  
  return 0;
}

void FTU_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  
  HAL_UART_Transmit(FTU.UartHander,Tx_Buff ,Tx_Size ,2000);
//  HAL_UART_Transmit_DMA(&huart3,Tx_Buff ,Tx_Size);
 //memset(Forward.USART3_Tx_Buff,0,Forward.USART3_Tx_Buff_Size);
 //Forward.USART3_Tx_Buff_Size = 0;
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );

}



void FTU_CopyToLora(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  Lora.TxSize = Rx_Size;
  memcpy(Lora.TxBuff, Rx_Buff, Rx_Size);
  FTU.RxEndFlag = 1;
  FTU.RxCount++;
  HAL_UART_Receive_DMA(FTU.UartHander,FTU.RxBuff,FTU_RX_MAX_SIZE);
}



