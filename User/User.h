#ifndef _USER_H_

#define _USER_H_

#define ROTATE 0
#define STEPMOTOR_MICRO_STEP      16
#define MOTO1_MEDIAN_LOCATION 31850  
#include "stm32f1xx_hal.h"
#include "main.h"
#include "fatfs.h"

#define USART3_RX_MAX_SIZE 500
#define USART2_RX_MAX_SIZE 500
#define USART1_RX_MAX_SIZE 500

#define USART2_TX_MAX_SIZE 100
#define USART3_TX_MAX_SIZE 500

#define SD_CARD_LOG 1 //1  开启TF卡 1
//#define Source node

/* 扩展变量 ------------------------------------------------------------------*/

typedef struct
{
  uint16_t Lora_ID;
  uint16_t Last_Lora_ID;
  uint16_t Target_Lora_ID;
 
  
  uint8_t SD_Status;
  uint8_t Key_ID_Status;
  
  uint8_t Rx_LED_Status;
  uint8_t Tx_LED_Status;
  
  uint8_t Head[3] ;
  uint8_t Tail[3] ;
  
//  uint8_t Rx_Head[3] ;
//  uint8_t Rx_Tail[3] ;
  uint8_t USART1_Tx_Buff[USART3_TX_MAX_SIZE];  
  uint8_t USART2_Tx_Buff[USART3_TX_MAX_SIZE];//[USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t USART3_Tx_Buff[USART3_TX_MAX_SIZE];
  
  uint8_t USART3_Rx_Buff[USART3_RX_MAX_SIZE];
  uint8_t USART2_Rx_Buff[USART2_RX_MAX_SIZE];
  uint8_t USART1_Rx_Buff[USART2_RX_MAX_SIZE];
  
  uint16_t USART3_Rx_Buff_Size; 
  uint16_t USART2_Rx_Buff_Size; 
  uint16_t USART1_Rx_Buff_Size; 
  
  uint16_t USART3_Tx_Buff_Size; 
  uint16_t USART2_Tx_Buff_Size; 
  uint16_t USART1_Tx_Buff_Size; 
  
  uint8_t USART3_Rx_End_Flag;  
  uint8_t USART2_Rx_End_Flag;  
  uint8_t USART1_Rx_End_Flag; 
  uint8_t Read_ID_Flag;
}Forward_t;


typedef struct
{
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  
  uint8_t Day;
  uint8_t Month;
  uint16_t Year;
  
  char Buff[20];
}DateTime_t;

extern RTC_TimeTypeDef GetTime; 
extern RTC_DateTypeDef GetData;  
extern uint8_t ReadConfigBuff[18];
extern Forward_t Forward;
extern DateTime_t DateTime;
extern FIL File;
extern char SD_FileName[] ;
extern char WriteBuffer[2000];
extern char LogBuff[100];

uint8_t Check(uint8_t *data , uint16_t n);
void User_Transmit(UART_HandleTypeDef *huart, uint8_t *SendBuf, uint16_t SendLen);
HAL_StatusTypeDef Read_Lora_ID(void);


void USAR3_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size);
void USAR2_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size);
void USART3_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size);
void USART2_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size);
void UserLoop(void);
void HexToStr(uint8_t *hex, uint16_t n, char *str);
uint8_t SDCardLogInit(void);
void Logging(const void *buff);
void WriteTarIDFlash(uint16_t TarID);
void ReadTarIDFlash(void);

#endif

