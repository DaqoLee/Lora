#ifndef _FTU_H_
#define _FTU_H_
#include "usart.h"

#define FTU_RX_MAX_SIZE 500

#define FTU_TX_MAX_SIZE 500



typedef struct
{

  uint8_t RxLEDStatus;
  uint8_t TxLEDStatus;
  

  uint8_t TxBuff[FTU_TX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t RxBuff[FTU_RX_MAX_SIZE];

  uint16_t RxSize; 
  uint16_t TxSize; 

  uint8_t RxEndFlag;  
  uint8_t ReadIDFlag;
  
}FTU_t;

extern FTU_t FTU;
extern UART_HandleTypeDef *FTU_UartHander;
int FTU_IRQHandler(void);
void FTU_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size);
void FTU_Init(void);
#endif


