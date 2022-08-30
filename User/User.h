#ifndef _USER_H_

#define _USER_H_
 
#include "stm32f1xx_hal.h"
#include "main.h"
#include "fatfs.h"
#include "usart.h"

#define USER_RX_MAX_SIZE 1000
#define USER_TX_MAX_SIZE 1000

#define USART3_RX_MAX_SIZE 500
#define USART2_RX_MAX_SIZE 500
#define USART1_RX_MAX_SIZE 500

#define USART2_TX_MAX_SIZE 100

#define USART3_TX_MAX_SIZE 500

#define SD_CARD_LOG 1 //1  开启TF卡 1
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
  uint8_t ReadIDFlag;
  UserStatus_t ErrCode;
  
  
}User_t;


//typedef struct
//{
//  uint16_t Lora_ID;
//  uint16_t Last_Lora_ID;
//  uint16_t Target_Lora_ID;
// 
//  
//  uint8_t SD_Status;
//  uint8_t Key_ID_Status;
//  
//  uint8_t Rx_LED_Status;
//  uint8_t Tx_LED_Status;
//  
//  uint8_t Head[3] ;
//  uint8_t Tail[3] ;
//  
////  uint8_t Rx_Head[3] ;
////  uint8_t Rx_Tail[3] ;
//  uint8_t USART1_Tx_Buff[USART3_TX_MAX_SIZE];  
//  uint8_t USART2_Tx_Buff[USART3_TX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
//  uint8_t USART3_Tx_Buff[USART3_TX_MAX_SIZE];
//  
//  uint8_t USART3_Rx_Buff[USART3_RX_MAX_SIZE];
//  uint8_t USART2_Rx_Buff[USART2_RX_MAX_SIZE];
//  uint8_t USART1_Rx_Buff[USART2_RX_MAX_SIZE];
//  
//  uint16_t USART3_Rx_Buff_Size; 
//  uint16_t USART2_Rx_Buff_Size; 
//  uint16_t USART1_Rx_Buff_Size; 
//  
//  uint16_t USART3_Tx_Buff_Size; 
//  uint16_t USART2_Tx_Buff_Size; 
//  uint16_t USART1_Tx_Buff_Size; 
//  
//  uint8_t USART3_Rx_End_Flag;  
//  uint8_t USART2_Rx_End_Flag;  
//  uint8_t USART1_Rx_End_Flag; 
//  uint8_t Read_ID_Flag;
//}Forward_t;


typedef struct
{
  char Buff[30];
}DateTime_t;

extern RTC_TimeTypeDef Time; 
extern RTC_DateTypeDef Date;  
extern User_t User;     
//extern Forward_t Forward;
extern DateTime_t DateTime;

extern char LogBuff[100];
extern UART_HandleTypeDef *User_UartHander;

//uint8_t Check(uint8_t *data , uint16_t n);
void User_Transmit(UART_HandleTypeDef *huart, uint8_t *SendBuf, uint16_t SendLen);
//HAL_StatusTypeDef Read_Lora_ID(void);


void UserLoop(void);
//void HexToStr(uint8_t *hex, uint16_t n, char *str);
//uint8_t SDCardLogInit(void);
//void Logging(const void *buff);
//void WriteTarIDFlash(uint16_t TarID);
//void ReadTarIDFlash(void);
int User_IRQHandler(void);
void User_Init(void);
#endif

