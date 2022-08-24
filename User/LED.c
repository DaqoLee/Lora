#include "LED.h"
#include "SDdriver.h"
#include "User.h"
#include "Lora.h"
void LEDStatus(uint16_t frequency)
{
  static uint16_t loraIDTemp=0;
//  static uint16_t loraTarIDTemp=0;
//  static uint16_t sdTemp=0;
  static uint16_t ledrToggle=0; 
//  static uint8_t ledbToggle=0; 
  
  if(Lora.ID == 0)
  {
    if( loraIDTemp++ >5)
    {
      loraIDTemp=0;
      HAL_GPIO_TogglePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin);   
    }    
  }
  else 
  {
    if(ledrToggle < Lora.ID && Lora.KeyIDStatus == 0)
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
     else if((ledrToggle - Lora.ID) < Lora.TargetID && Lora.KeyIDStatus == 0)
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
      if(Lora.KeyIDStatus == 0)
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
  
  if(SD_Status==0)
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


