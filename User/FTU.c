#include "FTU.h"
#include "User.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "Lora.h"

UART_HandleTypeDef *FTU_UartHander = &huart3;

FTU_t FTU={
            .RxLEDStatus = 0,
            .TxLEDStatus = 0,

            .TxBuff = {'0'},
            .RxBuff =  {'0'},

            .RxSize = 0,
            .TxSize = 0,
            .RxCount = 0,
            .ReadIDFlag = 0,
};



void FTU_CopyToLora(uint8_t *Rx_Buff, uint16_t Rx_Size);


void FTU_Init(void)
{
  __HAL_UART_CLEAR_IDLEFLAG(FTU_UartHander);//清除标志位
	__HAL_UART_ENABLE_IT(FTU_UartHander, UART_IT_IDLE );   
  HAL_UART_Receive_DMA(FTU_UartHander, FTU.RxBuff, FTU_RX_MAX_SIZE);
  
}

int FTU_IRQHandler(void)
{
  if(NULL == FTU_UartHander)
    return -1;
	if (__HAL_UART_GET_FLAG(FTU_UartHander, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(FTU_UartHander);//清除标志位
    HAL_UART_DMAStop(FTU_UartHander);
    
    FTU.RxSize = FTU_RX_MAX_SIZE - __HAL_DMA_GET_COUNTER(FTU_UartHander->hdmarx);// huart1.hdmarx->Instance->CNDTR;
    
    FTU_CopyToLora(FTU.RxBuff, FTU.RxSize);
  }
    
  
  return 0;
}

void FTU_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  
  HAL_UART_Transmit(FTU_UartHander,Tx_Buff ,Tx_Size ,2000);
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
  HAL_UART_Receive_DMA(FTU_UartHander,FTU.RxBuff,FTU_RX_MAX_SIZE);
}



