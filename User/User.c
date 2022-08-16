#include "User.h"
#include "tim.h"
#include "usart.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
   

uint8_t ReadConfigBuff[18]={0x01, 0x00, 0x02, 0x0D, 0xA5, 0xA5, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E };//01 00 02 0D A5 A5 00 00 00 00 00 00 00 00 00 00 00 0E

Forward_t Forward ={.Lora_ID = 0,
                    .Head = {0xAA, 0xAA, 0xAA},
                    .Tail = {0xEE, 0xEE, 0xEE},
                    
                    .USART2_Tx_Buff = {0},
                    .USART3_Tx_Buff = {0},
                    
                    .USART2_Rx_Buff = {0},
                    .USART3_Rx_Buff = {0},     
                    
                    .USART3_Rx_Buff_Size = 0,
                    .USART2_Rx_Buff_Size = 0,

                    .USART3_Tx_Buff_Size = 0,
                    .USART2_Tx_Buff_Size = 0,
                    
                    .USART3_Rx_End_Flag = 0,
                    .USART2_Rx_End_Flag = 0,
                    };




void USAR2_Pack_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size ,uint16_t ID)
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
  
  Tx_Data[Tx_Size + 10] = Check(Tx_Data, Tx_Size + 10);
  HAL_UART_Transmit(&huart2,Tx_Data ,Tx_Size+11 ,1000);
}



/*串口1接收数据，串口2发送数据 */
void USAR3_Transmit(void)
{
  
  HAL_UART_Transmit(&huart3,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size ,2000);
  
  memset(Forward.USART3_Tx_Buff,0,Forward.USART3_Tx_Buff_Size);
  //Forward.USART3_Tx_Buff_Size = 0;
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );

}

/*串口2接收数据，串口1发送数据 */
void USAR2_Transmit(void)
{
  uint8_t temp = 0;
  
  switch(Forward.Lora_ID)
  {  
    case 0x01 :
      
     if(Forward.USART2_Tx_Buff_Size >USART2_TX_MAX_SIZE)
     {
       USAR2_Pack_Transmit(Forward.Head ,sizeof(Forward.Head) ,0x03); 
       HAL_Delay(500);
       while(Forward.USART2_Tx_Buff_Size / USART2_TX_MAX_SIZE)
       {
         USAR2_Pack_Transmit(Forward.USART2_Tx_Buff[temp] ,USART2_TX_MAX_SIZE ,0x03);       
              
         HAL_Delay(1000);
         memset(Forward.USART2_Tx_Buff[temp],0,USART2_TX_MAX_SIZE);
         Forward.USART2_Tx_Buff_Size -= USART2_TX_MAX_SIZE; 
         temp++;
       }
      
       USAR2_Pack_Transmit(Forward.USART2_Tx_Buff[temp] ,Forward.USART2_Tx_Buff_Size ,0x03);
       HAL_Delay(1000);
       USAR2_Pack_Transmit(Forward.Tail ,sizeof(Forward.Tail) ,0x03);
       HAL_Delay(500);
     }
     else
     {
       USAR2_Pack_Transmit(Forward.USART2_Tx_Buff[temp] ,Forward.USART2_Tx_Buff_Size ,0x03);
       HAL_Delay(500);
     }

     memset(Forward.USART2_Tx_Buff[temp],0,Forward.USART2_Tx_Buff_Size);
    // Forward.USART2_Tx_Buff_Size = 0;
      break;
      
    case 0x02 :
      
      break;
    case 0x03 :     
    
     if(Forward.USART2_Tx_Buff_Size > USART2_TX_MAX_SIZE)
     {
       
       HAL_UART_Transmit(&huart2, Forward.Head, sizeof(Forward.Head) ,200);
       HAL_Delay(500);
       while(Forward.USART2_Tx_Buff_Size / USART2_TX_MAX_SIZE)
       {     
         HAL_UART_Transmit(&huart2,Forward.USART2_Tx_Buff[temp] ,USART2_TX_MAX_SIZE ,1000);       
              
         HAL_Delay(1000);
         memset(Forward.USART2_Tx_Buff[temp],0,USART2_TX_MAX_SIZE);
         Forward.USART2_Tx_Buff_Size -= USART2_TX_MAX_SIZE; 
         temp++;
       }
       HAL_UART_Transmit(&huart2,Forward.USART2_Tx_Buff[temp] ,Forward.USART2_Tx_Buff_Size ,1000);       
       HAL_Delay(1000);
       HAL_UART_Transmit(&huart2, Forward.Tail, sizeof(Forward.Tail) ,200);
       HAL_Delay(500);
     }
     else
     {
      HAL_UART_Transmit(&huart2,Forward.USART2_Tx_Buff[temp] ,Forward.USART2_Tx_Buff_Size ,1000);       
      HAL_Delay(1000);
     
     }

     memset(Forward.USART2_Tx_Buff[temp],0,Forward.USART2_Tx_Buff_Size);
     //Forward.USART2_Tx_Buff_Size = 0;
      break;   
    
    default:

      break;
  }
  // HAL_UART_Transmit(&huart2,Forward.USART2_Tx_Buff ,Forward.USART3_Tx_Buff_Size ,100);
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );
 
}


void USART3_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  uint8_t temp = 0;
  
  Forward.USART2_Tx_Buff_Size = Rx_Size;
  
  while(Rx_Size / USART2_TX_MAX_SIZE)
  {
    memcpy(Forward.USART2_Tx_Buff[temp], &Rx_Buff[temp * USART2_TX_MAX_SIZE], USART2_TX_MAX_SIZE);
    Rx_Size -= USART2_TX_MAX_SIZE;
    temp++;
  }    
  memcpy(Forward.USART2_Tx_Buff[temp], &Rx_Buff[temp * USART2_TX_MAX_SIZE], Rx_Size); 

  Forward.USART3_Rx_End_Flag = 1;
}


void USART2_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  
  static uint8_t status = 0;
  static uint8_t temp = 0;
    
  switch(Forward.Lora_ID)
  {  
    case 0x01 :
      if(Rx_Buff[0] == 0x05 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82 && Rx_Buff[Rx_Size-1] == Check(Rx_Buff,Rx_Size-1))
      {
        if(Rx_Buff[7] == 3 && Rx_Buff[8] == 0xAA && Rx_Buff[9] == 0xAA && Rx_Buff[10] == 0xAA)
        {
          status = 1;
        }
        else if(Rx_Buff[7] == 3 && Rx_Buff[8] == 0xEE && Rx_Buff[9] == 0xEE && Rx_Buff[10] == 0xEE)
        {
          status = 0;
         
          Forward.USART2_Rx_End_Flag = 1;
        }
        else if (status == 0)
        {
          Forward.USART3_Tx_Buff_Size = Rx_Buff[7];
          memcpy(Forward.USART3_Tx_Buff, &Rx_Buff[8],  Forward.USART3_Tx_Buff_Size);
          Forward.USART2_Rx_End_Flag = 1;
        }
        else if (status == 1)
        { 
          memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], &Rx_Buff[8], Rx_Buff[7]);
          Forward.USART3_Tx_Buff_Size += Rx_Buff[7];
          temp++;
          if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1)
          {
            status = 0;
            temp = 0;
          }
        } 
      }
      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
      break;
      
    case 0x02 :
      
      break;
    case 0x03 :
      if(Rx_Size == 3 && Rx_Buff[0] == 0xAA && Rx_Buff[1] == 0xAA && Rx_Buff[2] == 0xAA)
      {
        status=1;
      }
      else if(Rx_Size == 3 && Rx_Buff[0] == 0xEE && Rx_Buff[1] == 0xEE && Rx_Buff[2] == 0xEE)
      {
        status=0;
        Forward.USART2_Rx_End_Flag = 1;
       // Forward.USART3_Tx_Buff_Size=0;
      }
      else if (status == 0)
      {
        Forward.USART3_Tx_Buff_Size = Rx_Size;
        memcpy(Forward.USART3_Tx_Buff, Rx_Buff,  Forward.USART3_Tx_Buff_Size); 
        Forward.USART2_Rx_End_Flag = 1;        
      }
      else if (status == 1)
      {
         memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], Rx_Buff, Rx_Size);
         Forward.USART3_Tx_Buff_Size += Rx_Size;
         temp++;
         if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE)
         {
           status = 0;
           temp = 0;
         }
      } 
      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
      break;   

    case 0x00 :
      if(Rx_Buff[0] == 0x01 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82)
      {
         if(Rx_Buff[Rx_Size-1] == Check(Rx_Buff,Rx_Size-1))
         {
            Forward.Lora_ID = Rx_Buff[Rx_Size - 5] | Rx_Buff[Rx_Size - 4];
         }
      } 
      break;      
      
    default:
      if(Rx_Size == 3 && Rx_Buff[0] == 0xAA && Rx_Buff[1] == 0xAA && Rx_Buff[2] == 0xAA)
      {
        status=1;
      }
      else if(Rx_Size == 3 && Rx_Buff[0] == 0xEE && Rx_Buff[1] == 0xEE && Rx_Buff[2] == 0xEE)
      {
        status=0;
       
       // Forward.USART3_Tx_Buff_Size=0;
        Forward.USART2_Rx_End_Flag = 1;
      }
      else if (status == 0)
      {
        Forward.USART3_Tx_Buff_Size = Rx_Size;
        memcpy(Forward.USART3_Tx_Buff, Rx_Buff,  Forward.USART3_Tx_Buff_Size);  
        Forward.USART2_Rx_End_Flag = 1;        
      }
      else if (status == 1)
      {
         memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], &Rx_Buff, Rx_Size);
         Forward.USART3_Tx_Buff_Size += Rx_Size;       
         temp++;
         if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1)
         {
           status = 0;
           temp = 0;
         }
      } 
      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
      break;
  }
    
}


void Read_Lora_ID(void)
{
  do
  {
    printf("Read_Lora_ID\r\n");
    HAL_Delay(500);
    HAL_UART_Transmit(&huart2, ReadConfigBuff, sizeof(ReadConfigBuff),500);
    
   // HAL_UART_Transmit_DMA(&huart2, ReadConfigBuff, sizeof(ReadConfigBuff));
    HAL_Delay(500);
    HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  }while(Forward.Lora_ID == 0);
  
}


uint8_t Check(uint8_t *data , uint16_t n)
{
  uint8_t temp=0;
  
  temp = data[0];
  for(int i=1; i<n; i++)
  {
    temp^=data[i];  
  }
  return temp;
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//更新中断（溢出）发生时执行
{
  if(htim->Instance == TIM3 )
	{

		
	}
}



