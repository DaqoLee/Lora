#ifndef _USER_H_

#define _USER_H_

#define ROTATE 0
#define STEPMOTOR_MICRO_STEP      16
#define MOTO1_MEDIAN_LOCATION 31850  
#include "stm32f1xx_hal.h"
#include "main.h"

#define USART1_RX_MAX_SIZE 500
#define USART2_RX_MAX_SIZE 500

#define USART2_TX_MAX_SIZE 100
#define USART1_TX_MAX_SIZE 500

//#define Source node

/* ¿©’π±‰¡ø ------------------------------------------------------------------*/

typedef struct
{
  uint16_t Lora_ID;
  
  uint8_t Head[3] ;
  uint8_t Tail[3] ;
  
//  uint8_t Rx_Head[3] ;
//  uint8_t Rx_Tail[3] ;
  
  uint8_t USART2_Tx_Buff[USART1_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1][USART2_TX_MAX_SIZE] ;
  uint8_t USART1_Tx_Buff[USART1_TX_MAX_SIZE];
  
  uint8_t USART1_Rx_Buff[USART1_RX_MAX_SIZE];
  uint8_t USART2_Rx_Buff[USART2_RX_MAX_SIZE];
  
  uint16_t USART1_Rx_Buff_Size; 
  uint16_t USART2_Rx_Buff_Size; 
 
  uint16_t USART1_Tx_Buff_Size; 
  uint16_t USART2_Tx_Buff_Size; 
  
  uint8_t USART1_Rx_End_Flag;  
  uint8_t USART2_Rx_End_Flag;  
  
}Forward_t;




extern uint8_t ReadConfigBuff[18];
extern Forward_t Forward;



uint8_t Check(uint8_t *data , uint16_t n);
void User_Transmit(UART_HandleTypeDef *huart, uint8_t *SendBuf, uint16_t SendLen);
void Read_Lora_ID(void);


void USAR1_Transmit(void);
void USAR2_Transmit(void);
void USART1_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size);
void USART2_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size);
#endif

