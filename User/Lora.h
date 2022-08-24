#ifndef _LORA_H_
#define _LORA_H_
#include "usart.h"

#define LORA_RX_MAX_SIZE 500

#define LORA_TX_MAX_SIZE 100


typedef struct
{
  uint16_t ID;
  uint16_t LastID;
  uint16_t TargetID;
 
  uint8_t KeyIDStatus;
  
  uint8_t RxLEDStatus;
  uint8_t TxLEDStatus;
  

  uint8_t TxBuff[LORA_RX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t RxBuff[LORA_RX_MAX_SIZE];

  uint16_t RxSize; 
  uint16_t TxSize; 

  uint8_t RxEndFlag;  
  uint8_t ReadIDFlag;
  
}Lora_t;



extern UART_HandleTypeDef *Lora_UartHander;
extern Lora_t Lora;
int Lora_IRQHandler(void);
void Lora_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size);
void Lora_ReadTargetIDFromFlash(void);
HAL_StatusTypeDef Lora_ReadID(void);
void Lora_SetIDWithKey(void);
void Lora_Init(void);
#endif



