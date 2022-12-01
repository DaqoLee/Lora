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
   Date_write_BKP(&hrtc,&DateToUpdate);//�����ڱ�����BKP�����ֹ�����ʧ
  }
  else
  {
  
  /*��ΪRTC�Ǹ��ݰ���һ��24*60*365����м���ʱ�䣬Ҳ����24Сʱ��
		hrtc.DateToUpdate�����ڱ��������ҳ�ʼ��Ϊ0��1��1��
		������HAL_RTC_GetTime�����������ж��Ƿ񳬹�һ�죬�������ڱ��沢��hrtc.DateToUpdate�У�
		Ȼ�����ֵ���ȥһ��������������Ͳ���ͨ��������ֵ��������˶�����
		������ʱ����Ҳ�Ͷ�ʧ�ˣ�Ҳ�������ھͶ�ʧ��ֻҪ��ʱ�����ڱ����ں�����
		�ϵ���ȶ�ȡ���������³�ʼ��hrtc.DateToUpdate������
		������HAL_RTC_GetTime�Ϳ��Ը���hrtc.DateToUpdate�����������ڣ������Ͳ��ᶪʧ������
		*/
		//�Ѿ���ʼ������Ϊ�����ó�ʼ���������Ǳ����ø�ֵ�Ļ���Ҫ��ֵ
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
		Date_read_BKP(&hrtc);//��ȡBKP����ĵ���ǰ�����ڣ�Ȼ����ݼ���ֵ��������ڼ���˼���
		//32λÿ�����1�Ļ����Լ���136�꣬���õ������
  
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
{//�����ڱ����ڱ�����
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR2,Date->WeekDay);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR3,Date->Year);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR4,Date->Month);
  HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR5,Date->Date);
}
void Date_read_BKP(RTC_HandleTypeDef *  hrtc)
{//�����ڴӱ�����ԭ��hrtc->DateToUpdate����HAL_RTC_GetDate��������
  Date.WeekDay=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR2);
  Date.Year=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR3);
  Date.Month=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR4);
  Date.Date=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR5);
  HAL_RTC_SetDate(hrtc, &Date, RTC_FORMAT_BIN);
//���õ�hrtc.DateToUpdate�� ��ΪHAL_RTC_GetTimeֻ����DateToUpdate ��ʵ���Ƕඨ����һ��Date����ֱ��ʹ��hrtc��DateToUpdate
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
