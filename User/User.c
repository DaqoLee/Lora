#include "User.h"
#include "tim.h"
#include "usart.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "SDdriver.h"   
#include "key.h"
#include "Flash.h"

uint8_t ReadConfigBuff[18]={0x01, 0x00, 0x02, 0x0D, 0xA5, 0xA5, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E };//01 00 02 0D A5 A5 00 00 00 00 00 00 00 00 00 00 00 0E

Forward_t Forward ={.Lora_ID = 0,
                    .Last_Lora_ID=0,
                    .Target_Lora_ID = 1,
                    .SD_Status=0,
                    .Key_ID_Status=0,
  
                    .Rx_LED_Status=0,
                    .Tx_LED_Status=0,
  
                    .Head = {0xAA, 0xAA, 0xAA},
                    .Tail = {0xEE, 0xEE, 0xEE},

                    .USART1_Tx_Buff = {0},                    
                    .USART2_Tx_Buff = {0},
                    .USART3_Tx_Buff = {0},
                    
                    .USART1_Rx_Buff = {0},                    
                    .USART2_Rx_Buff = {0},
                    .USART3_Rx_Buff = {0},     
                    
                    .USART3_Rx_Buff_Size = 0,
                    .USART2_Rx_Buff_Size = 0,

                    .USART3_Tx_Buff_Size = 0,
                    .USART2_Tx_Buff_Size = 0,
                    
                    .USART3_Rx_End_Flag = 0,
                    .USART2_Rx_End_Flag = 0,
                    .USART1_Rx_End_Flag = 0,
                    .Read_ID_Flag = 0,
                    };



DateTime_t DateTime = {.Second = 0,
                       .Minute = 0,
                       .Hour = 0,
  
                       .Day = 18,
                       .Month = 8,
                       .Year = 2020,
                      };

RTC_TimeTypeDef GetTime; 
RTC_DateTypeDef GetData;                       
                      
FIL File;
char SD_FileName[] = "2022-08-19_11-31-27";
char WriteBuffer[2000]={'0'};
char LogBuff[100]={'0'};
  FATFS FS;
  UINT Bw;	

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
//  HAL_UART_Transmit_DMA(&huart2,Tx_Data ,Tx_Size+11);
}



void USAR3_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  
  HAL_UART_Transmit(&huart3,Tx_Buff ,Tx_Size ,2000);
//  HAL_UART_Transmit_DMA(&huart3,Tx_Buff ,Tx_Size);
 //memset(Forward.USART3_Tx_Buff,0,Forward.USART3_Tx_Buff_Size);
 //Forward.USART3_Tx_Buff_Size = 0;
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );

}


void USAR2_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  uint8_t temp = 0;
//  char tempbuff[500];
//  switch(Forward.Lora_ID)
//  {  
//    case 0x01 :
//      
     if(Tx_Size >USART2_TX_MAX_SIZE)
     {
       USAR2_Pack_Transmit(Forward.Head ,sizeof(Forward.Head) ,Forward.Target_Lora_ID); 
       HAL_Delay(500);
       while(Tx_Size / USART2_TX_MAX_SIZE)
       {
         USAR2_Pack_Transmit(&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,USART2_TX_MAX_SIZE ,Forward.Target_Lora_ID);       
              
         HAL_Delay(1000);
       //  memset(Tx_Buff[temp],0,USART2_TX_MAX_SIZE);
         Tx_Size -= USART2_TX_MAX_SIZE; 
         temp++;
       }
      
       USAR2_Pack_Transmit(&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,Tx_Size ,Forward.Target_Lora_ID);
       HAL_Delay(1000);
       USAR2_Pack_Transmit(Forward.Tail ,sizeof(Forward.Tail) ,Forward.Target_Lora_ID);
       HAL_Delay(500);
       temp = 0;
     }
     else
     {
       USAR2_Pack_Transmit(Tx_Buff ,Tx_Size ,Forward.Target_Lora_ID);
       HAL_Delay(500);
     }

    // memset(Tx_Buff[temp],0,Tx_Size);
    // Tx_Size = 0;
//      break;
      
//    case 0x02 :
//     if(Tx_Size > USART2_TX_MAX_SIZE)
//     {
//       
//       HAL_UART_Transmit(&huart2, Forward.Head, sizeof(Forward.Head) ,200);
//       HAL_Delay(500);
//       while(Tx_Size / USART2_TX_MAX_SIZE)
//       {     
//         HAL_UART_Transmit(&huart2,&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,USART2_TX_MAX_SIZE ,1000);          
//         HAL_Delay(1000);
//        // memset(Tx_Buff[temp],0,USART2_TX_MAX_SIZE);
//         Tx_Size -= USART2_TX_MAX_SIZE; 
//         temp++;
//       }
//       HAL_UART_Transmit(&huart2,&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,Tx_Size ,1000);       
//       HAL_Delay(1000);
//       HAL_UART_Transmit(&huart2, Forward.Tail, sizeof(Forward.Tail) ,200);
//       HAL_Delay(500);
//     }
//     else
//     {
//      HAL_UART_Transmit(&huart2,Tx_Buff, Tx_Size, 1000);      
//      HAL_UART_Transmit(&huart1,Tx_Buff, Tx_Size, 500);       
//      HAL_Delay(1000);
//     
//     }
//      break;
//    case 0x03 :     
//    
//     if(Tx_Size > USART2_TX_MAX_SIZE)
//     {
//       
//       HAL_UART_Transmit(&huart2, Forward.Head, sizeof(Forward.Head) ,200);
//       HAL_Delay(500);
//       while(Tx_Size / USART2_TX_MAX_SIZE)
//       {     
//         HAL_UART_Transmit(&huart2,&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,USART2_TX_MAX_SIZE ,1000);          
//         HAL_Delay(1000);
//        // memset(Tx_Buff[temp],0,USART2_TX_MAX_SIZE);
//         Tx_Size -= USART2_TX_MAX_SIZE; 
//         temp++;
//       }
//       HAL_UART_Transmit(&huart2,&Tx_Buff[temp * USART2_TX_MAX_SIZE] ,Tx_Size ,1000);       
//       HAL_Delay(1000);
//       HAL_UART_Transmit(&huart2, Forward.Tail, sizeof(Forward.Tail) ,200);
//       HAL_Delay(500);
//     }
//     else
//     {
//      HAL_UART_Transmit(&huart2,Tx_Buff ,Tx_Size ,1000);       
//      HAL_Delay(1000);
//     
//     }

//     //memset(Tx_Buff[temp],0,Tx_Size);
//     //Tx_Size = 0;
//      break;   
//    
//    default:

//      break;
//  }
  // HAL_UART_Transmit(&huart2,Tx_Buff ,Forward.USART3_Tx_Buff_Size ,100);
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );
 
}


void USART3_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  Forward.USART2_Tx_Buff_Size = Rx_Size;
  memcpy(Forward.USART2_Tx_Buff, Rx_Buff, Rx_Size);
  Forward.USART3_Rx_End_Flag = 1;
}


void USART2_Rx_Analysis(uint8_t *Rx_Buff, uint16_t Rx_Size)
{
  
  static uint8_t status = 0;
  static uint8_t temp = 0;
//  sprintf(LogBuff,"%s:\t USART2 receive %d byte \r\n",DateTime.Buff,Rx_Size);
//  Logging(LogBuff);
//  switch(Forward.Lora_ID)
//  {  
//    case 0x01 :
 
    if(Rx_Buff[0] == 0x05 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82 && Rx_Buff[Rx_Size-1] == Check(Rx_Buff,Rx_Size-1))
    {
      if(Rx_Buff[7] == 3 && Rx_Buff[8] == 0xAA && Rx_Buff[9] == 0xAA && Rx_Buff[10] == 0xAA)
      {
        status = 1;
        temp = 0;
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
    else if(Rx_Buff[0] == 0x01 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82)
    {
       if(Rx_Buff[Rx_Size-1] == Check(Rx_Buff,Rx_Size-1))
       {
          Forward.Lora_ID = Rx_Buff[Rx_Size - 5] | Rx_Buff[Rx_Size - 4];
         
       }
    } 
    
  

     HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
//      break;
//      
//    case 0x02 :
//      if(Rx_Size == 3 && Rx_Buff[0] == 0xAA && Rx_Buff[1] == 0xAA && Rx_Buff[2] == 0xAA)
//      {
//        status=1;
//      }
//      else if(Rx_Size == 3 && Rx_Buff[0] == 0xEE && Rx_Buff[1] == 0xEE && Rx_Buff[2] == 0xEE)
//      {
//        status=0;
//        Forward.USART2_Rx_End_Flag = 1;
//       // Forward.USART3_Tx_Buff_Size=0;
//      }
//      else if (status == 0)
//      {
//        Forward.USART3_Tx_Buff_Size = Rx_Size;
//        memcpy(Forward.USART3_Tx_Buff, Rx_Buff,  Forward.USART3_Tx_Buff_Size); 
//        Forward.USART2_Rx_End_Flag = 1;        
//      }
//      else if (status == 1)
//      {
//         memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], Rx_Buff, Rx_Size);
//         Forward.USART3_Tx_Buff_Size += Rx_Size;
//         temp++;
//         if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE)
//         {
//           status = 0;
//           temp = 0;
//         }
//      } 
//      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
//      break;         
//    case 0x03 :
//      if(Rx_Size == 3 && Rx_Buff[0] == 0xAA && Rx_Buff[1] == 0xAA && Rx_Buff[2] == 0xAA)
//      {
//        status=1;
//      }
//      else if(Rx_Size == 3 && Rx_Buff[0] == 0xEE && Rx_Buff[1] == 0xEE && Rx_Buff[2] == 0xEE)
//      {
//        status=0;
//        Forward.USART2_Rx_End_Flag = 1;
//       // Forward.USART3_Tx_Buff_Size=0;
//      }
//      else if (status == 0)
//      {
//        Forward.USART3_Tx_Buff_Size = Rx_Size;
//        memcpy(Forward.USART3_Tx_Buff, Rx_Buff,  Forward.USART3_Tx_Buff_Size); 
//        Forward.USART2_Rx_End_Flag = 1;        
//      }
//      else if (status == 1)
//      {
//         memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], Rx_Buff, Rx_Size);
//         Forward.USART3_Tx_Buff_Size += Rx_Size;
//         temp++;
//         if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE)
//         {
//           status = 0;
//           temp = 0;
//         }
//      } 
//      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
//      break;   

//    case 0x00 :

//      break;      
//      
//    default:
//      if(Rx_Size == 3 && Rx_Buff[0] == 0xAA && Rx_Buff[1] == 0xAA && Rx_Buff[2] == 0xAA)
//      {
//        status=1;
//      }
//      else if(Rx_Size == 3 && Rx_Buff[0] == 0xEE && Rx_Buff[1] == 0xEE && Rx_Buff[2] == 0xEE)
//      {
//        status=0;
//       
//       // Forward.USART3_Tx_Buff_Size=0;
//        Forward.USART2_Rx_End_Flag = 1;
//      }
//      else if (status == 0)
//      {
//        Forward.USART3_Tx_Buff_Size = Rx_Size;
//        memcpy(Forward.USART3_Tx_Buff, Rx_Buff,  Forward.USART3_Tx_Buff_Size);  
//        Forward.USART2_Rx_End_Flag = 1;        
//      }
//      else if (status == 1)
//      {
//         memcpy(&Forward.USART3_Tx_Buff[Forward.USART3_Tx_Buff_Size], &Rx_Buff, Rx_Size);
//         Forward.USART3_Tx_Buff_Size += Rx_Size;       
//         temp++;
//         if(temp > USART3_RX_MAX_SIZE/USART2_TX_MAX_SIZE + 1)
//         {
//           status = 0;
//           temp = 0;
//         }
//      } 
//      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
//      break;
//  }
    
}


HAL_StatusTypeDef Read_Lora_ID(void)
{
  uint16_t temp = 10;
  do
  {
    Logging("Read_Lora_ID\r\n");
    HAL_Delay(500);
    HAL_UART_Transmit(&huart2,ReadConfigBuff, sizeof(ReadConfigBuff),500);
//    HAL_UART_Transmit_DMA(&huart2, ReadConfigBuff, sizeof(ReadConfigBuff));
    HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  }while(Forward.Lora_ID == Forward.Last_Lora_ID && temp--);
  
  Forward.Last_Lora_ID = Forward.Lora_ID;
  if(temp <= 0)
  {
    return HAL_TIMEOUT;
  }
  else
  {
    return HAL_OK;
  } 
}


void Write_Lora_ID(uint16_t ID)
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
  Tx_Data[17] = Check(Tx_Data,17);
  
  HAL_UART_Transmit(&huart2,Tx_Data ,18 ,1000);
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


void HexToStr(uint8_t *hex, uint16_t n, char *str)
{
  for (int i = 0; i < n; i++)
  {
    
    sprintf(str + i * 3, "%02x ", hex[i]); //之所以i*2 是因为 0x73 转换为字符串73，所占空间会多一个
  }
  strcat(str, "\r\n");
}

void DateTimeUpdate(uint8_t frequency)
{
  static uint8_t temp=0;
  
  temp++;  
  if(temp >= frequency)
  {
    temp = 0;
    DateTime.Second++;
    if(DateTime.Second >= 60 )
    { 
      DateTime.Second = 0;
      DateTime.Minute ++;
      if(DateTime.Minute >= 60 )
      {
        DateTime.Minute = 0;
        DateTime.Hour ++;
        if(DateTime.Hour >= 24 )
        {
          DateTime.Hour = 0;
          DateTime.Day ++;
               
        }  
        if(DateTime.Day > 31 )
        {
          DateTime.Day = 1;
          DateTime.Month ++;
          if(DateTime.Month > 12 )
          {
            DateTime.Month = 1;
            DateTime.Year ++;                
          }
               
        }
      }  
    }
    sprintf(DateTime.Buff,"%02d-%02d-%02d %02d-%02d-%02d",DateTime.Year, DateTime.Month, DateTime.Day, DateTime.Hour, DateTime.Minute, DateTime.Second); 
   
    //printf("%s\r\n",DateTime.Buff);
    // printf("%d %d %d  %d %d %d\r\n",DateTime.Year, DateTime.Month, DateTime.Day, DateTime.Hour, DateTime.Minute, DateTime.Second);
  }
}



uint8_t SDCardLogInit()
{
  uint8_t res=0;


  if(SD_init() == FR_OK)
  {
    Forward.SD_Status = 1;
    printf("初始化成功！ \r\n");			
  }
  else
  {
    Forward.SD_Status = 0;
    printf("初始化失败！ \r\n");
  }	
	
  if(f_mount(&FS,"0:",1) == FR_OK)
  {
    Forward.SD_Status = 1;
    printf("文件挂载成功！ \r\n");			
  }
  else
  {
    Forward.SD_Status = 0;
    printf("文件挂载失败！ \r\n");
  }	
  sprintf(SD_FileName,"%02d-%02d-%02d_%02d-%02d-%02d.txt",2000+GetData.Year, GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds); 
  if(f_open(&File,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE) == FR_OK)//创建文件 
  {
    printf("文件创建成功！ \r\n");	
    Forward.SD_Status = 1;		
  }
  else
  {
    printf("文件创建失败！ \r\n");
    Forward.SD_Status = 0;
  }		
  f_lseek(&File, f_size(&File));
  res = f_write(&File,DateTime.Buff,sizeof(DateTime.Buff),&Bw);
  if(res == FR_OK)
  {
    printf("文件写入成功！ \r\n");	
    Forward.SD_Status = 1;		
  }
  else
  {
    printf("文件写入失败！ \r\n");
    Forward.SD_Status = 0;
  }		
  f_sync(&File);
  return res;
}

void SD_Write(const void *buff)
{
  	uint8_t res=0;
    UINT Bw;	

//    if(f_open(&File, SD_FileName, FA_OPEN_ALWAYS|FA_WRITE) == FR_OK)
//    {
//      printf("文件打开成功！ \r\n");			
//    }
//    else
//    {
//      printf("文件打开失败！ \r\n");
//    }
    f_lseek(&File, f_size(&File));
    if(f_write(&File,buff,strlen(buff),&Bw) == FR_OK)
    {
     // printf("文件写入成功！ \r\n");
      Forward.SD_Status = 1;			
    }
    else
    {
      // printf("文件写入失败！ \r\n");
      Forward.SD_Status = 0;
    }		
    f_sync(&File);
  
    res= res;
}

void LEDStatus(uint16_t frequency)
{
  static uint16_t loraIDTemp=0;
//  static uint16_t loraTarIDTemp=0;
//  static uint16_t sdTemp=0;
  static uint8_t ledrToggle=0; 
//  static uint8_t ledbToggle=0; 
  
  if(Forward.Lora_ID == 0)
  {
    if( loraIDTemp++ >5)
    {
      loraIDTemp=0;
      HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin);   
    }    
  }
  else 
  {
    if(ledrToggle < Forward.Lora_ID && Forward.Key_ID_Status == 0)
    {
       loraIDTemp++;
      if(loraIDTemp < frequency/4)
      {
        
        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
        
      }
      else if(loraIDTemp < frequency/2)
      {           
        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
      }
      else
      {
        loraIDTemp = 0;
        ledrToggle++;   
      }
    }
     else if((ledrToggle - Forward.Lora_ID) < Forward.Target_Lora_ID && Forward.Key_ID_Status == 0)
    {
       loraIDTemp++;
      if(loraIDTemp < frequency/4)
      {
        
        HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET);
        
      }
      else if(loraIDTemp < frequency/2)
      {           
        HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_RESET);
      }
      else
      {
        loraIDTemp = 0;
        ledrToggle++;   
      }
    }   
    else
    {
      if(Forward.Key_ID_Status == 0)
      {
        loraIDTemp = 0;
        ledrToggle = 0;  
      }
      else if(loraIDTemp >= frequency)
      {
        loraIDTemp = 0;
        ledrToggle = 0;         
      } 
      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_RESET);
      loraIDTemp++;      
    } 
    
    
//    if(ledbToggle < Forward.Target_Lora_ID)
//    {
//       loraTarIDTemp++;
//      if(loraTarIDTemp < frequency/4)
//      {
//        
//        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
//        
//      }
//      else if(loraTarIDTemp < frequency/2)
//      {           
//        HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
//      }
//      else
//      {
//        loraTarIDTemp = 0;
//        ledbToggle++;   
//      }
//    }
//    else
//    {
//      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
//      loraTarIDTemp++;
//      if(loraTarIDTemp >= frequency)
//      {
//        loraTarIDTemp = 0;
//        ledbToggle = 0;         
//      }   
//    } 
  }
  
  if(Forward.SD_Status==0)
  {
//   if( sdTemp++ >5)
//    {
//      sdTemp=0;
//      HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin); 
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);    
//    }    
  }
  else
  {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
  }
  
//  if(Forward.Rx_LED_Status==1)
//  {
//    sdTemp++;
////    HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin); 
//    if(sdTemp < frequency/8)
//    {
//      
//      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
//      
//    }
//    else if(sdTemp < frequency/4)
//    {           
//      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
//    }
//    else
//    {
//      sdTemp = 0;
//      Forward.Rx_LED_Status=0;
//      //HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
//    }
//   if( sdTemp++ >9)
//    {
//      sdTemp=0;
//       Forward.Rx_LED_Status=0;
//    }    
//  }

}

void Logging(const void *buff)
{
  
#if SD_CARD_LOG  

  SD_Write(buff);
#else
  printf("%s", (char *)buff);
#endif    

}

void UserLoop(void)
{
  static uint8_t temp=0;
  if( Forward.USART2_Rx_End_Flag == 1)
  {
    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Forward.USART2_Rx_Buff_Size);
    Logging(LogBuff);
    USAR3_Transmit(Forward.USART3_Tx_Buff, Forward.USART3_Tx_Buff_Size);
    HAL_Delay(500);
    HAL_UART_Transmit(&huart1,Forward.Head,sizeof(Forward.Head),500);
    HAL_UART_Transmit(&huart1,Forward.USART3_Tx_Buff,Forward.USART3_Tx_Buff_Size,500);
    sprintf(LogBuff,"%s:\t USART3 transmit %d byte \r\n",DateTime.Buff,Forward.USART3_Tx_Buff_Size );
    Logging(LogBuff);
    Forward.USART2_Rx_End_Flag = 0;     
    Forward.USART2_Rx_Buff_Size = 0;
    Forward.USART3_Tx_Buff_Size = 0;
    memset(Forward.USART2_Rx_Buff,0,USART2_RX_MAX_SIZE);
    HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  }
  
  if( Forward.USART3_Rx_End_Flag == 1)
  {
    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Forward.USART3_Rx_Buff_Size);
    Logging(LogBuff);      
    USAR2_Transmit(Forward.USART2_Tx_Buff, Forward.USART2_Tx_Buff_Size);
    
    HAL_UART_Transmit(&huart1,Forward.Tail,sizeof(Forward.Tail),500);
    HAL_UART_Transmit(&huart1,Forward.USART2_Tx_Buff,Forward.USART2_Tx_Buff_Size,500);
    
    sprintf(LogBuff,"%s:\t USART2 transmit %d byte \r\n",DateTime.Buff,Forward.USART2_Tx_Buff_Size );
    Logging(LogBuff);     
    Forward.USART3_Rx_End_Flag = 0;
    Forward.USART3_Rx_Buff_Size = 0;
    Forward.USART2_Tx_Buff_Size = 0;
    memset(Forward.USART3_Rx_Buff,0,USART3_RX_MAX_SIZE);
    HAL_UART_Receive_DMA(&huart3,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
  }
  
//  if( Forward.SD_Status == 0)
//  {  
//    if( temp++ >200)
//    {
//      SDCardLogInit();
//      temp = 0;
//    } 
//  }
  
  if(Forward.Read_ID_Flag == 1)
  {
    Forward.Read_ID_Flag = 0;
    if(Read_Lora_ID() == HAL_OK)
    {
       printf("Lora ID 获取成功！ \r\n");	
    }
    else
    {
       printf("Lora ID 获取失败！ \r\n");
    }
  }
}


void SetLoraID(void)
{

   static uint8_t loraId = 0;
   static uint8_t tarLoraId = 0;
   static uint16_t timer=0;
  
   Key_Process();
  

//  if(userKeyList[KEY_1].KeyStatus)
//  {
//    if(keyTemp==0)
//    {
//      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);    
//    }
//    else if(keyTemp==1)
//    {
//      HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET); 
//    }
//    
//  }
  if(userKeyList[KEY_1].longPressedJump)
  {
    if(  Forward.Key_ID_Status ++ > 2)
    {
      Forward.Key_ID_Status = 1;
    }
  
  }
  
  switch(Forward.Key_ID_Status)
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
            Write_Lora_ID(loraId);
            Forward.Read_ID_Flag = 1;
            //Read_Lora_ID();
            //Forward.Lora_ID = loraId;
          }
          Forward.Key_ID_Status = 0;
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
            Forward.Target_Lora_ID = tarLoraId;
            WriteTarIDFlash(Forward.Target_Lora_ID);
          }
          Forward.Key_ID_Status = 0;
          timer = 0;
          HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_RESET);
        }
      }
      
    break;
  
  }
  
}


void WriteTarIDFlash(uint16_t TarID)
{
  uint8_t data[4];
  
  data[0] = TarID>>8;
  data[1] = TarID;  

  Flash_Write((u32*)data,1);
}

void ReadTarIDFlash(void)
{
  uint8_t data[4];
  Flash_Read((u32*)data,1);
  
  Forward.Target_Lora_ID = data[0]<<8 | data[1];
  
}

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
    HAL_RTC_GetTime(hrtc, &GetTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtc, &GetData, RTC_FORMAT_BIN);
  
  
   sprintf(DateTime.Buff,"%02d/%02d/%02d-%02d:%02d:%02d",2000+GetData.Year, GetData.Month, GetData.Date, GetTime.Hours, GetTime.Minutes, GetTime.Seconds); 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//10ms更新中断（溢出）发生时执行
{
  if(htim->Instance == TIM3 )
	{

		LEDStatus(100);
   SetLoraID();
    
	}

}



