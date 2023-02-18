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

DateTime_t DateTime;
User_t User ={
               .TxBuff = {0},
               .RxBuff= {0},
               .UartHander = &huart1,
							 .TxFlag = 0,
               .IwdgFlag = 1,
};                      
                      
RTC_TimeTypeDef Time; 
RTC_DateTypeDef Date;                       

int User_IRQHandler(void)
{
  if(NULL == User.UartHander)
    return -1;
	if (__HAL_UART_GET_FLAG(User.UartHander, UART_FLAG_IDLE) == SET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(User.UartHander);//清除标志位
    HAL_UART_DMAStop(User.UartHander);
    
    User.RxSize = USER_RX_MAX_SIZE - __HAL_DMA_GET_COUNTER(User.UartHander->hdmarx);// huart1.hdmarx->Instance->CNDTR;
    
#if UPEER_PC_800MN
   HAL_UART_Transmit(Lora.UartHander, User.RxBuff, User.RxSize, 500);
#endif
    
    User.RxEndFlag = 1;
    HAL_UART_Receive_DMA(User.UartHander,User.RxBuff, USER_RX_MAX_SIZE);

  }
    
  
  return 0;
}

void User_Init(void)
{
  __HAL_UART_CLEAR_IDLEFLAG(User.UartHander);//清除标志位
  __HAL_UART_ENABLE_IT(User.UartHander,UART_IT_IDLE );
  HAL_UART_Receive_DMA(User.UartHander,User.RxBuff, USER_RX_MAX_SIZE);
 
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
//  if( FTU.RxEndFlag == 1)
//  {
////    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, FTU.RxSize);
////    Logging(LogBuff);      
//    Lora_Transmit(Lora.TxBuff,Lora.TxSize);
////   HAL_Delay(50);
////    
////    HAL_UART_Transmit(User.UartHander, head,sizeof(head), 500);
////    HAL_UART_Transmit(User.UartHander, Lora.TxBuff, Lora.TxSize, 500);
////    
////    sprintf(LogBuff,"%s:\t USART2 transmit %d byte \r\n",DateTime.Buff,Lora.TxSize );
////    Logging(LogBuff);     
//    
//    FTU.RxEndFlag = 0;
//    FTU.RxSize = 0;
//    Lora.TxSize =0;

//    memset(FTU.RxBuff, 0, FTU_RX_MAX_SIZE);
////    HAL_UART_Receive_DMA(FTU_UartHander, FTU.RxBuff, FTU_RX_MAX_SIZE);
//  }

  if(FTU.pFTU_RXBuff_Out != FTU.pFTU_RXBuff_In)
  {
    Lora_Transmit(FTU.pFTU_RXBuff_Out->pStart, FTU.pFTU_RXBuff_Out->pEnd - FTU.pFTU_RXBuff_Out->pStart);  

#if DEBUG   
    HAL_UART_Transmit(User.UartHander, head,sizeof(head), 500);
    HAL_UART_Transmit(User.UartHander, FTU.pFTU_RXBuff_Out->pStart, FTU.pFTU_RXBuff_Out->pEnd - FTU.pFTU_RXBuff_Out->pStart, 500);
#endif 
    
    FTU.pFTU_RXBuff_Out++;
    if(FTU.pFTU_RXBuff_Out == FTU.pFTU_RXBuff_End)
    {
      FTU.pFTU_RXBuff_Out = FTU.FTU_RXBuff_Pointer;
    }
  }
  
}

void LoraTransmitToFUT(void)
{
  static uint8_t head[]={0x22, 0x22, 0x22};
  if( Lora.RxEndFlag == 1)
  {
//    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Lora.RxSize);
//    Logging(LogBuff);
//    HAL_Delay(50);
    FTU_Transmit(FTU.TxBuff,FTU.TxSize);
//    HAL_Delay(50);
#if DEBUG   
    HAL_UART_Transmit(User.UartHander, head,sizeof(head), 500);
    HAL_UART_Transmit(User.UartHander, FTU.TxBuff, FTU.TxSize, 500);
#endif    
//    sprintf(LogBuff,"%s:\t USART3 transmit %d byte \r\n",DateTime.Buff, FTU.TxSize );
//    Logging(LogBuff);
    Lora.RxEndFlag = 0;
    Lora.RxSize = 0;
    FTU.TxSize = 0;

    memset(Lora.RxBuff,0,LORA_RX_MAX_SIZE);
//    HAL_UART_Receive_DMA(Lora_UartHander,Lora.RxBuff,LORA_RX_MAX_SIZE);
  }

}

void LoraTransmitToUser(void)
{
  static uint8_t head[]={0x22, 0x22, 0x22};
  if( User.TxFlag == 1)
  {
//    sprintf(LogBuff,"%s:\t USART2 receive  %d byte \r\n",DateTime.Buff, Lora.RxSize);
//    Logging(LogBuff);
//    HAL_Delay(50);
//    FTU_Transmit(FTU.TxBuff,FTU.TxSize);
#if UPEER_PC_800MN
  	HAL_UART_Transmit(User.UartHander, User.TxBuff, User.TxSize, 1000);
		User.TxFlag =0;
#endif		

//    HAL_Delay(50);
//#if DEBUG   
//    HAL_UART_Transmit(User.UartHander, head,sizeof(head), 500);
//    HAL_UART_Transmit(User.UartHander, FTU.TxBuff, FTU.TxSize, 500);
//#endif    
//    sprintf(LogBuff,"%s:\t USART3 transmit %d byte \r\n",DateTime.Buff, FTU.TxSize );
//    Logging(LogBuff);
//    Lora.RxEndFlag = 0;
//    Lora.RxSize = 0;
//    FTU.TxSize = 0;

//    memset(Lora.RxBuff,0,LORA_RX_MAX_SIZE);
//    HAL_UART_Receive_DMA(Lora_UartHander,Lora.RxBuff,LORA_RX_MAX_SIZE);
  }

}

void UserLoop(void)
{
//  static uint8_t temp=0;
//  static uint32_t tick = 0;
  
  FUTTransmitToLora();
	LoraTransmitToUser();
  LoraTransmitToFUT();
  
//  if( SD_Status == 0)
//  {  
//    if( temp++ >200)
//    {
//      SDCardLogInit();
//      temp = 0;
//    } 
//  }
  
  if(Lora.ReadIDStatus == READ_ID_START)
  {
   
    if(Lora_ReadID() == S_OK)
    {
       printf("Lora ID 获取成功！ \r\n");	
    }
    else
    {
       printf("Lora ID 获取失败！ \r\n");
    }
     Lora.ReadIDStatus = LORA_OK;
  }
  
  if(Lora.RxCount == 0 || FTU.RxCount ==  0)
  {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);  
  }
  else
  {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);  
  }
//  tick = HAL_GetTick();
////  SDCardLogInit();
//  
//  
////  printf("%02d/%02d/%02d-%02d:%02d:%02d\r\n",2000+Date.Year, Date.Month, Date.Date, Time.Hours, Time.Minutes, Time.Seconds); 
// 
//  sprintf(DateTime.Buff,"%02d/%02d/%02d-%02d:%02d:%02d",2000+Date.Year, Date.Month, Date.Date, Time.Hours, Time.Minutes, Time.Seconds); 
//  sprintf(LogBuff,"%s:  Lora no response!\tError code: %X\r\n",DateTime.Buff, Lora.ReplyStatus);
//  Logging(LogBuff);
//  printf("%d\r\n",HAL_GetTick()-tick);
  
//  if(Lora.LogStatus == LOG_START)
//  {
//    sprintf(DateTime.Buff,"%02d/%02d/%02d-%02d:%02d:%02d",2000+Date.Year, Date.Month, Date.Date, Time.Hours, Time.Minutes, Time.Seconds); 
//    sprintf(LogBuff,"%s:  Lora no response!\tError code: %X\r\n",DateTime.Buff, Lora.ReplyCode);
//    Logging(LogBuff);
//    Lora.LogStatus = LORA_OK;
//  }
}






void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//10ms更新中断（溢出）发生时执行
{
  if(htim->Instance == TIM3 )
	{

		LEDStatus(100);
    Lora_SetIDWithKey();
    
    if(Lora.RxTime++ > (LORA_FTU_RX_TIME_OUT *100) || FTU.RxTime++ > (LORA_FTU_RX_TIME_OUT *100))
    {
      User.IwdgFlag = 0;
    }
    else
    {
      User.IwdgFlag = 1;
    }
    
	}

}



