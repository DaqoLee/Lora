#include "User.h"
#include "tim.h"
#include <math.h>
#include "stdio.h"
#include "string.h"
#include "SDdriver.h"   
#include "key.h"
#include "Flash.h"
#include "LED.h"
#include "Lora.h"
#include "FTU.h"
UART_HandleTypeDef *User_UartHander = &huart1;


DateTime_t DateTime = {.Second = 0,
                       .Minute = 0,
                       .Hour = 0,
  
                       .Day = 18,
                       .Month = 8,
                       .Year = 2020,
                      };

User_t User;                      
                      
RTC_TimeTypeDef GetTime; 
RTC_DateTypeDef GetData;                       
                      




int User_IRQHandler(void)
{
  if(NULL == User_UartHander)
    return -1;
	if (__HAL_UART_GET_FLAG(User_UartHander, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(User_UartHander);//清除标志位
    HAL_UART_DMAStop(User_UartHander);
    
    User.RxSize = USER_RX_MAX_SIZE - __HAL_DMA_GET_COUNTER(User_UartHander->hdmarx);// huart1.hdmarx->Instance->CNDTR;
    HAL_UART_Transmit(Lora_UartHander, User.RxBuff, User.RxSize, 500);
    User.RxEndFlag = 1;
    HAL_UART_Receive_DMA(User_UartHander,User.RxBuff, USER_RX_MAX_SIZE);

  }
    
  
  return 0;
}

void User_Init(void)
{
  __HAL_UART_CLEAR_IDLEFLAG(User_UartHander);//清除标志位
  __HAL_UART_ENABLE_IT(User_UartHander,UART_IT_IDLE );
  HAL_UART_Receive_DMA(User_UartHander,User.RxBuff, USER_RX_MAX_SIZE);
 
}






void Logging(const void *buff)
{
  
#if SD_CARD_LOG  

  SD_Write(buff);
#else
  printf("%s", (char *)buff);
#endif    

}


void FUTTransmitToLora(void)
{
  static uint8_t head[]={0x33, 0x33, 0x33};
  if( FTU.RxEndFlag == 1)
  {
    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, FTU.RxSize);
    Logging(LogBuff);      
    Lora_Transmit(Lora.TxBuff,Lora.TxSize);
    
    HAL_UART_Transmit(User_UartHander, head,sizeof(head), 500);
    HAL_UART_Transmit(User_UartHander, Lora.TxBuff, Lora.TxSize, 500);
    
    sprintf(LogBuff,"%s:\t USART2 transmit %d byte \r\n",DateTime.Buff,Lora.TxSize );
    Logging(LogBuff);     
    
    FTU.RxEndFlag = 0;
    FTU.RxSize = 0;
    Lora.TxSize =0;

    memset(FTU.RxBuff, 0, FTU_RX_MAX_SIZE);
    HAL_UART_Receive_DMA(FTU_UartHander, FTU.RxBuff, FTU_RX_MAX_SIZE);
  }

}

void LoraTransmitToFUT(void)
{
  static uint8_t head[]={0x22, 0x22, 0x22};
  if( Lora.RxEndFlag == 1)
  {
    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Lora.RxSize);
    Logging(LogBuff);
    FTU_Transmit(FTU.TxBuff,FTU_TX_MAX_SIZE);
    HAL_Delay(500);
    
    HAL_UART_Transmit(User_UartHander, head,sizeof(head), 500);
    HAL_UART_Transmit(User_UartHander, FTU.TxBuff, FTU.TxSize, 500);
    
    sprintf(LogBuff,"%s:\t USART3 transmit %d byte \r\n",DateTime.Buff, FTU.TxSize );
    Logging(LogBuff);
    Lora.RxEndFlag = 0;
    Lora.RxSize = 0;
    FTU.TxSize = 0;

    memset(Lora.RxBuff,0,LORA_RX_MAX_SIZE);
    HAL_UART_Receive_DMA(Lora_UartHander,Lora.RxBuff,LORA_RX_MAX_SIZE);
  }

}
void UserLoop(void)
{
  static uint8_t temp=0;

  
  FUTTransmitToLora();
  LoraTransmitToFUT();
  
  if( SD_Status == 0)
  {  
    if( temp++ >200)
    {
      SDCardLogInit();
      temp = 0;
    } 
  }
  
  if(Lora.ReadIDFlag == 1)
  {
    Lora.ReadIDFlag = 0;
    if(Lora_ReadID() == HAL_OK)
    {
       printf("Lora ID 获取成功！ \r\n");	
    }
    else
    {
       printf("Lora ID 获取失败！ \r\n");
    }
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
    Lora_SetIDWithKey();
    
	}

}



