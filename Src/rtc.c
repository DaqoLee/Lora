/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "string.h"
extern RTC_TimeTypeDef Time; 
extern RTC_DateTypeDef Date;         

void Date_read_BKP(RTC_HandleTypeDef *  hrtc);
void Date_write_BKP(RTC_HandleTypeDef *  hrtc,RTC_DateTypeDef * Date);
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1)!=0x5020)
  {

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x9;
  sTime.Minutes = 0x35;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_THURSDAY;
  DateToUpdate.Month = RTC_MONTH_AUGUST;
  DateToUpdate.Date = 0x26;
  DateToUpdate.Year = 0x22;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

   HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5020);//!=0x5050
   Date_write_BKP(&hrtc,&DateToUpdate);//将日期保存在BKP区域防止掉电后丢失
  }
  else
  {
  
  /*因为RTC是根据按照一天24*60*365秒进行计算时间，也就是24小时制
		hrtc.DateToUpdate的用于保存日期且初始化为0年1月1日
		当调用HAL_RTC_GetTime根据秒数，判断是否超过一天，更新日期保存并在hrtc.DateToUpdate中，
		然后计数值会减去一天的秒数，这样就不能通过读计数值算出来过了多少天
		当掉电时变量也就丢失了，也就是日期就丢失了只要定时将日期保存在后备区域，
		上电后先读取后备区域重新初始化hrtc.DateToUpdate变量，
		当调用HAL_RTC_GetTime就可以根据hrtc.DateToUpdate变量更新日期，这样就不会丢失日期了
		*/
		//已经初始化过因为不调用初始化函数但是变量该赋值的还是要赋值
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
		Date_read_BKP(&hrtc);//读取BKP区域的掉电前的日期，然后根据计数值计算掉电期间过了几天
		//32位每秒计数1的话可以计数136年，不用担心溢出
  
  }
  
	HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&Date,RTC_FORMAT_BIN);
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


void Date_write_BKP(RTC_HandleTypeDef *  hrtc,RTC_DateTypeDef * Date)
{//将日期保存在备份域
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR2,Date->WeekDay);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR3,Date->Year);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR4,Date->Month);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR5,Date->Date);
}
void Date_read_BKP(RTC_HandleTypeDef *  hrtc)
{//将日期从备份域还原到hrtc->DateToUpdate用于HAL_RTC_GetDate更新日期
  Date.WeekDay=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR2);
  Date.Year=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR3);
  Date.Month=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR4);
  Date.Date=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR5);
  HAL_RTC_SetDate(hrtc, &Date, RTC_FORMAT_BIN);
//设置到hrtc.DateToUpdate中 因为HAL_RTC_GetTime只更新DateToUpdate 其实我是多定义了一个Date可以直接使用hrtc的DateToUpdate
}

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
  static uint8_t Time_Date=0xFF;
  HAL_RTC_GetTime(hrtc, &Time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(hrtc, &Date, RTC_FORMAT_BIN);

//  printf("%02d/%02d/%02d-%02d:%02d:%02d\r\n",2000+Date.Year, Date.Month, Date.Date, Time.Hours, Time.Minutes, Time.Seconds); 
  if(Time_Date!=Date.Date)
  {
    Time_Date=Date.Date;
    Date_write_BKP(hrtc,&Date);
  } 
  
  
}

/* USER CODE END 1 */
