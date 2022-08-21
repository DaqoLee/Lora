#include "User.h"
#include "tim.h"
#include "usart.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "SDdriver.h"   


uint8_t ReadConfigBuff[18]={0x01, 0x00, 0x02, 0x0D, 0xA5, 0xA5, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E };//01 00 02 0D A5 A5 00 00 00 00 00 00 00 00 00 00 00 0E

Forward_t Forward ={.Lora_ID = 0,
                    .Target_Lora_ID = 1,
                    .SD_Status=0,
  
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
char SD_FileName[] = "0:LOG001.txt";
char WriteBuffer[2000]={'0'};
char LogBuff[100]={'0'};

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
void USAR3_Transmit(uint8_t *Tx_Buff, uint16_t Tx_Size)
{
  
  HAL_UART_Transmit(&huart3,Tx_Buff ,Tx_Size ,2000);
  
 //memset(Forward.USART3_Tx_Buff,0,Forward.USART3_Tx_Buff_Size);
 //Forward.USART3_Tx_Buff_Size = 0;
 // HAL_UART_Transmit_DMA(&huart1,Forward.USART3_Tx_Buff ,Forward.USART3_Tx_Buff_Size );

}

/*串口2接收数据，串口1发送数据 */
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
  if(Forward.Lora_ID ==0)
  {
    if(Rx_Buff[0] == 0x01 && Rx_Buff[1] == 0x00 && Rx_Buff[2] == 0x82)
    {
       if(Rx_Buff[Rx_Size-1] == Check(Rx_Buff,Rx_Size-1))
       {
          Forward.Lora_ID = Rx_Buff[Rx_Size - 5] | Rx_Buff[Rx_Size - 4];
         
         if(Forward.Lora_ID == 1)
         {
           Forward.Target_Lora_ID = 3;
         
         }
         else
         {
           Forward.Target_Lora_ID = 1;
         }
       }
    } 
  }
  else
  {
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
  uint16_t temp = 0;
  do
  {
    Logging("Read_Lora_ID\r\n");
    HAL_Delay(500);
    HAL_UART_Transmit(&huart2, ReadConfigBuff, sizeof(ReadConfigBuff),500);
    
    if(temp++ >10)
    {
      break;    
    }
   // HAL_UART_Transmit_DMA(&huart2, ReadConfigBuff, sizeof(ReadConfigBuff));
    HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  }while(Forward.Lora_ID == 0);
  
  if(temp >10)
  {
    return HAL_TIMEOUT;
  }
  else
  {
    return HAL_OK;
  } 
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



uint8_t SDCardLogInit(FIL* fp, const void *buff)
{
  uint8_t res=0;
  FATFS FS;
  UINT Bw;	

	res = SD_init();		//SD卡初始化
  
	res = f_mount(&FS,"0:",1);		//挂载
  if(res == FR_OK)
  {
    printf("文件挂载成功！ \r\n");			
  }
  else
  {
    printf("文件挂载失败！ \r\n");
  }	
 // res = f_getfree("0:", &fre_clust, &fs);	
  
  res = f_open(fp,buff,FA_OPEN_ALWAYS |FA_WRITE);//创建文件 
  if(res == FR_OK)
  {
    printf("文件创建成功！ \r\n");			
  }
  else
  {
    printf("文件创建失败！ \r\n");
  }		
  res = f_write(fp,DateTime.Buff,sizeof(DateTime.Buff),&Bw);
  if(res == FR_OK)
  {
    printf("文件写入成功！ \r\n");			
  }
  else
  {
    printf("文件写入失败！ \r\n");
  }		
  f_close(fp);
//  res = f_open(fp, SD_FileName, FA_OPEN_ALWAYS|FA_WRITE);
//  if(res == FR_OK)
//  {
//    printf("文件打开成功！ \r\n");			
//  }
//  else
//  {
//    printf("文件打开失败！ \r\n");
//  }
 // res = f_open(fp,buff,FA_OPEN_ALWAYS |FA_WRITE);//创建文件 
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
    //  printf("文件写入成功！ \r\n");			
    }
    else
    {
   //   printf("文件写入失败！ \r\n");
    }		
    f_sync(&File);
  
//    res = f_open(fp, SD_FileName, FA_OPEN_ALWAYS|FA_WRITE);
//    if(res == FR_OK)
//		{
//			printf("文件打开成功！ \r\n");			
//		}
//		else
//		{
//			printf("文件打开失败！ \r\n");
//		}	
   // f_lseek(fp, f_size(fp));
//    res = f_write(fp, buff , btw, bw);
//  	if(res == FR_OK)
//		{
//			printf("文件写入成功！ \r\n");			
//		}
//		else
//		{
//			printf("文件写入失败！ \r\n");
//		}		
//    f_sync(fp);
 //   f_close(fp);   
    res= res;
}

void LEDStatus(uint16_t frequency)
{
  static uint16_t loraTemp=0;
  static uint16_t sdTemp=0;
  static uint8_t ledToggle=0; 
 
  if(Forward.Lora_ID == 0)
  {
    if( loraTemp++ >5)
    {
      loraTemp=0;
      HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);   
    }    
  }
  else
  {
    if(ledToggle < Forward.Lora_ID)
    {
       loraTemp++;
      if(loraTemp < frequency/4)
      {
        
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
        
      }
      else if(loraTemp < frequency/2)
      {           
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
      }
      else
      {
        loraTemp = 0;
        ledToggle++;   
      }
    }
    else
    {
      HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
      loraTemp++;
      if(loraTemp >= frequency)
      {
        loraTemp = 0;
        ledToggle = 0;         
      }
    
    } 
  }
  
//  if(Forward.SD_Status==0)
//  {
//   if( sdTemp++ >5)
//    {
//      sdTemp=0;
//      HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin);   
//    }    
//  }
  
  if(Forward.Rx_LED_Status==1)
  {
    sdTemp++;
//    HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin); 
    if(sdTemp < frequency/8)
    {
      
      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
      
    }
    else if(sdTemp < frequency/4)
    {           
      HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
    }
    else
    {
      sdTemp = 0;
      Forward.Rx_LED_Status=0;
      //HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_RESET);
    }
//   if( sdTemp++ >9)
//    {
//      sdTemp=0;
//       Forward.Rx_LED_Status=0;
//    }    
  }

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
  if( Forward.USART2_Rx_End_Flag == 1)
  {
    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Forward.USART2_Rx_Buff_Size);
    Logging(LogBuff);
    USAR3_Transmit(Forward.USART3_Tx_Buff, Forward.USART3_Tx_Buff_Size);
    HAL_Delay(500);
//    HAL_UART_Transmit(&huart1,Forward.USART3_Tx_Buff,Forward.USART3_Tx_Buff_Size,500);
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
//    HAL_UART_Transmit(&huart1,Forward.USART2_Tx_Buff,Forward.USART2_Tx_Buff_Size,500);
    sprintf(LogBuff,"%s:\t USART2 transmit %d byte \r\n",DateTime.Buff,Forward.USART2_Tx_Buff_Size );
    Logging(LogBuff);     
    Forward.USART3_Rx_End_Flag = 0;
    Forward.USART3_Rx_Buff_Size = 0;
    Forward.USART2_Tx_Buff_Size = 0;
    memset(Forward.USART3_Rx_Buff,0,USART3_RX_MAX_SIZE);
    HAL_UART_Receive_DMA(&huart3,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
  }
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
	}

}



