#ifndef _LORA_H_
#define _LORA_H_
#include "usart.h"
#include "User.h"

#define LORA_RX_MAX_SIZE 600

#define LORA_TX_MAX_SIZE 100


typedef enum {
	LORA_OK = 0,				
  LORA_ERROR = 1,
  
  READ_ID_START = 2,
  READ_ID_END = 3, 
  READ_ID_ERR = 4,
  
  LOG_START = 5,
  LOG_END = 6,
  
  KEY_SET_ID_START = 7,
  KEY_SET_ID_END = 8,
  
  REPLY_OK = 9,
  REPLY_RST = 10,

} LoraStatus_t;

typedef struct
{
  uint16_t ID;
  uint16_t LastID;
  uint16_t TargetID;
  
  LoraStatus_t ReadIDStatus; 
  LoraStatus_t LogStatus;
  LoraStatus_t KeyIDStatus;
  LoraStatus_t ReplyStatus;
  
  uint8_t RxLEDStatus;
  uint8_t TxLEDStatus;
  
  uint8_t ReplyCode;   

  uint8_t TxBuff[LORA_RX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t RxBuff[LORA_RX_MAX_SIZE];

  uint16_t RxSize; 
  uint16_t TxSize; 

  uint16_t RxCount;
  uint32_t RxTime;
  
  uint8_t RxEndFlag;  

  UART_HandleTypeDef *UartHander;
  
}Lora_t;



extern Lora_t Lora;
int Lora_IRQHandler(void);
void Lora_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size);
void Lora_ReadTargetIDFromFlash(void);
LoraStatus_t Lora_ReadID(void);
void Lora_SetIDWithKey(void);
void Lora_Init(void);
#endif



