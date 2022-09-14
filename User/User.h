#ifndef _USER_H_

#define _USER_H_
 
#include "stm32f1xx_hal.h"
#include "main.h"
#include "fatfs.h"
#include "usart.h"

#define USER_RX_MAX_SIZE 1000
#define USER_TX_MAX_SIZE 1000


#define DEBUG 0
#define UPEER_PC_800MN 1

#define SD_CARD_LOG 0 //1  开启TF卡 1

//#define Source node

/* 扩展变量 ------------------------------------------------------------------*/

typedef enum {
	S_OK = 0,				/* (0) Succeeded */
 // READ_ID_ERR = 1,
  

} UserStatus_t;

typedef struct
{

  uint8_t RxLEDStatus;
  uint8_t TxLEDStatus;
  

  uint8_t TxBuff[USER_TX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t RxBuff[USER_RX_MAX_SIZE];

  uint16_t RxSize; 
  uint16_t TxSize; 

  uint8_t RxEndFlag; 
	uint8_t TxFlag; 
	
  uint8_t ReadIDFlag;
  UserStatus_t ErrCode;
  
  UART_HandleTypeDef *UartHander;
  
}User_t;


typedef struct
{
  char Buff[30];
}DateTime_t;

extern RTC_TimeTypeDef Time; 
extern RTC_DateTypeDef Date;  
extern User_t User;     
extern DateTime_t DateTime;
extern char LogBuff[100];

void User_Transmit(UART_HandleTypeDef *huart, uint8_t *SendBuf, uint16_t SendLen);

void UserLoop(void);
int User_IRQHandler(void);
void User_Init(void);
#endif

