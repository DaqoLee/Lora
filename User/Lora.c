#include "Lora.h"
#include "User.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "key.h"
#include "FTU.h"
#include "Flash.h"


UART_HandleTypeDef *Lora_UartHander = &huart2;

Lora_t Lora = {.ID = 0,
               .LastID = 0,
               .TargetID = 1,

               .KeyIDStatus = 0,
                
               .RxLEDStatus = 0,
               .TxLEDStatus = 0,

               .TxBuff = {0},
               .RxBuff= {0},
                
               . RxSize = 0,
               . TxSize = 0,
                
               .RxEndFlag = 0, 
               .ReadIDFlag = 0            
};

uint8_t Lora_Check(uint8_t *data , uint16_t n);
void Lora_CopyToFTU(uint8_t *Rx_Buff, uint16_t Rx_Size);


int Lora_IRQHandler(void)
{
  if(NULL == Lora_UartHander)
    return -1;
	if (__HAL_UART_GET_FLAG(Lora_UartHander, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(Lora_UartHander);//清除标志位
    HAL_UART_DMAStop(Lora_UartHander);
    
    Lora.RxSize = LORA_RX_MAX_SIZE - __HAL_DMA_GET_COUNTER(Lora_UartHander->hdmarx);// huart1.hdmarx->Instance->CNDTR;
    if(User.RxEndFlag == 1)
    {
      User.RxEndFlag = 0;
      HAL_UART_Transmit(User_UartHander,Lora.RxBuff,Lora.RxSize,500);  
     //  HAL_UART_Transmit_DMA(&huart1, Forward.USART2_Rx_Buff,Forward.USART2_Rx_Buff_Size);
    //   HAL_UART_Receive_DMA(Lora_UartHander,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
      
    } 
    else
    {
  //    HAL_UART_Transmit(&huart1,Forward.Head,sizeof(Forward.Head),500);
  //    HAL_UART_Transmit(&huart1,Forward.USART2_Rx_Buff,Forward.USART2_Rx_Buff_Size,500);
  //  
    }
    

    Lora_CopyToFTU(Lora.RxBuff, Lora.RxSize);
  }
    
  return 0;
}


void Lora_Init(void)
{

  __HAL_UART_CLEAR_IDLEFLAG(Lora_UartHander);//清除标志位
	__HAL_UART_ENABLE_IT(Lora_UartHander,UART_IT_IDLE );   
  HAL_UART_Receive_DMA(Lora_UartHander,Lora.RxBuff,LORA_RX_MAX_SIZE);
  
  HAL_Delay(200);
  Lora_ReadID();
//  if(Lora_ReadID() == HAL_OK)
//  {
//     printf("Lora ID 获取成功！ \r\n");	
//  }
//  else
//  {
//     printf("Lora ID 获取失败！ \r\n");
//  }
  
  Lora_ReadTargetIDFromFlash();
}


void Lora_Pack(uint8_t *Tx_Buff, uint16_t Tx_Size ,uint16_t ID)
{
  uint8_t Tx_Data[Tx_Size + 11];
  
  Tx_Data[0] = 0x05;
  Tx_Data[1] = 0x00;
  Tx_Data[2] = 0x01;
  Tx_Data[3] = Tx_Size + 6;
  Tx_Data[4] = ID>>8;
  Tx_Data[5] = ID;
  Tx_Data[6] = 0x00;
  Tx_Data[7] = 0x07;
  Tx_Data[8] = 0x01;
  Tx_Data[9] = Tx_Size;
  memcpy(&Tx_Data[10], Tx_Buff, Tx_Size);
  
  Tx_Data[Tx_Size + 10] = Lora_Check(Tx_Data, Tx_Size + 10);
  HAL_UART_Transmit(Lora_UartHander,Tx_Data ,Tx_Size+11 ,1000);
//  HAL_UART_Transmit_DMA(Lora_UartHander,Tx_Data ,Tx_Size+11);
}


void Lora_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  uint8_t temp = 0;
  static uint8_t head[] = {0xAA, 0xAA, 0xAA};
  static uint8_t tail[] = {0xEE, 0xEE, 0xEE};
 
  if(Tx_Size >LORA_TX_MAX_SIZE)
  {
     Lora_Pack(head ,sizeof(head) ,Lora.TargetID); 
     HAL_Delay(500);
     while(Tx_Size / LORA_TX_MAX_SIZE)
     {
       Lora_Pack(&Tx_Buff[temp * LORA_TX_MAX_SIZE] ,LORA_TX_MAX_SIZE ,Lora.TargetID);       
            
       HAL_Delay(1000);
     //  memset(Tx_Buff[temp],0,LORA_TX_MAX_SIZE);
       Tx_Size -= LORA_TX_MAX_SIZE; 
       temp++;
     }

     Lora_Pack(&Tx_Buff[temp * LORA_TX_MAX_SIZE] ,Tx_Size ,Lora.TargetID);
     HAL_Delay(1000);
     Lora_Pack(tail ,sizeof(tail) ,Lora.TargetID);
     HAL_Delay(500);
     temp = 0;
  }
  else
  {
     Lora_Pack(Tx_Buff ,Tx_Size ,Lora.TargetID);
     HAL_Delay(500);
  }
 
}


void Lora_CopyToFTU(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  
  static uint8_t status = 0;
  static uint8_t temp = 0;

  if(Rx_Buff[0] == 0x05 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82 && Rx_Buff[Rx_Size-1] == Lora_Check(Rx_Buff,Rx_Size-1))
  {
    if(Rx_Buff[7] == 3 && Rx_Buff[8] == 0xAA && Rx_Buff[9] == 0xAA && Rx_Buff[10] == 0xAA)
    {
      status = 1;
      temp = 0;
    }
    else if(Rx_Buff[7] == 3 && Rx_Buff[8] == 0xEE && Rx_Buff[9] == 0xEE && Rx_Buff[10] == 0xEE)
    {
      status = 0;      
      Lora.RxEndFlag = 1;
    }
    else if (status == 0)
    {
      FTU.TxSize = Rx_Buff[7];
      memcpy(FTU.TxBuff, &Rx_Buff[8], FTU.TxSize);
      Lora.RxEndFlag = 1;
    }
    else if (status == 1)
    { 
      memcpy(&FTU.TxBuff[FTU.TxSize], &Rx_Buff[8], Rx_Buff[7]);
      FTU.TxSize += Rx_Buff[7];
      temp++;
      if(temp > FTU_RX_MAX_SIZE/LORA_TX_MAX_SIZE + 1)
      {
        status = 0;
        temp = 0;
      }
    } 
  }
  else if(Rx_Buff[0] == 0x01 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82)
  {
     if(Rx_Buff[Rx_Size-1] == Lora_Check(Rx_Buff,Rx_Size-1))
     {
        Lora.ID = Rx_Buff[Rx_Size - 5] | Rx_Buff[Rx_Size - 4];
       
     }
  }  

  HAL_UART_Receive_DMA(Lora_UartHander, Lora.RxBuff, LORA_RX_MAX_SIZE);    
}


HAL_StatusTypeDef Lora_ReadID(void)
{
  uint16_t temp = 10;
  static uint8_t ReadConfigBuff[18]={0x01, 0x00, 0x02, 0x0D, 0xA5, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E };
  do
  {
  //  Logging("Read_Lora_ID\r\n");
    HAL_Delay(500);
    HAL_UART_Transmit(Lora_UartHander,ReadConfigBuff, sizeof(ReadConfigBuff),500);
//    HAL_UART_Transmit_DMA(Lora_UartHander, ReadConfigBuff, sizeof(ReadConfigBuff));
    HAL_UART_Receive_DMA(Lora_UartHander,Lora.RxBuff,LORA_RX_MAX_SIZE);
  }while(Lora.ID == Lora.LastID && temp--);
  
   Lora.LastID = Lora.ID;
  if(temp <= 0)
  {
    return HAL_TIMEOUT;
  }
  else
  {
    return HAL_OK;
  } 
}


void Lora_WriteID(uint16_t ID)
{
  uint8_t Tx_Data[18];
  
  Tx_Data[0] = 0x01;
  Tx_Data[1] = 0x00;
  Tx_Data[2] = 0x01;
  Tx_Data[3] = 0x0D;
  Tx_Data[4] = 0xA5;
  Tx_Data[5] = 0XA5;
  Tx_Data[6] = 0x6C;
  Tx_Data[7] = 0x80;
  Tx_Data[8] = 0x12;
  Tx_Data[9] = 0x07;
  Tx_Data[10] = 0X17;
  Tx_Data[11] = 0x00;
  Tx_Data[12] = 0x00;
  Tx_Data[13] = ID>>8;
  Tx_Data[14] = ID;  
  Tx_Data[15] = 0x03;
  Tx_Data[16] = 0x00;
  Tx_Data[17] = Lora_Check(Tx_Data,17);
  
  HAL_UART_Transmit(Lora_UartHander,Tx_Data ,18 ,1000);
}


void Lora_WriteTargetIDToFlash(uint16_t TarID)
{
  uint8_t data[4];
  
  data[0] = TarID>>8;
  data[1] = TarID;  

  Flash_Write((u32*)data,1);
}

void Lora_ReadTargetIDFromFlash(void)
{
  uint8_t data[4];
  Flash_Read((u32*)data,1);
  
  Lora.TargetID = data[0]<<8 | data[1];
  
}

void Lora_SetIDWithKey(void)
{

   static uint8_t loraId = 0;
   static uint8_t tarLoraId = 0;
   static uint16_t timer=0;
  
   Key_Process();
  
  if(userKeyList[KEY_1].longPressedJump)
  {
    if(  Lora.KeyIDStatus ++ > 2)
    {
      Lora.KeyIDStatus = 1;
    }
  
  }
  
  switch(Lora.KeyIDStatus)
  {
    case 0:
      loraId = 0;
      tarLoraId = 0;

    break;
    
    case 1:
      
      if(userKeyList[KEY_1].keyPressedJump)
      {       
        loraId++;
        timer = 0;     
       
      }
      else if(userKeyList[KEY_1].longPressed)
      {         
        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
      }
      else if(userKeyList[KEY_1].KeyStatus)
      {         
        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
      }
      else 
      { 
        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
        if( timer++ > 200)
        {
          if(loraId > 0)
          {
            Lora_WriteID(loraId);
            Lora.ReadIDFlag = 1;
            //Read_Lora_ID();
            //Forward.Lora_ID = loraId;
          }
           Lora.KeyIDStatus = 0;
          timer = 0;
          HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
        }
      }
    break;
        
    case 2:
      
      if(userKeyList[KEY_1].keyPressedJump)
      {       
        tarLoraId++;
        timer = 0;     
       
      }
      else if(userKeyList[KEY_1].longPressed)
      {         
        HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET);
      }
      else if(userKeyList[KEY_1].KeyStatus)
      {         
        HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_RESET);
      }
      else 
      { 
        HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET);
        if( timer++ > 200)
        {
          if(tarLoraId > 0)
          {
            Lora.TargetID = tarLoraId;
            Lora_WriteTargetIDToFlash(Lora.TargetID);
          }
           Lora.KeyIDStatus = 0;
          timer = 0;
          HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_RESET);
        }
      }
      
    break;
  
  }
  
}

uint8_t Lora_Check(uint8_t *data , uint16_t n)
{
  uint8_t temp=0;
  
  temp = data[0];
  for(int i=1; i<n; i++)
  {
    temp^=data[i];  
  }
  return temp;
}

