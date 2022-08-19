/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "User.h"
#include "stdio.h"
#include "string.h"
#include "SDdriver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//FIL file;
//char SD_FileName[] = "0:Test18.txt";
//char WriteBuffer[2000]={'0'};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
 #if SD_CARD_LOG  
  uint8_t res=0;
  FATFS FS;
  UINT Bw;	
#endif
//  uint16_t FileSize ;
//  uint16_t temp=0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(500);  
  HAL_TIM_Base_Start_IT(&htim3);  
  
#if SD_CARD_LOG  
//  SDCardLogInit(&File, SD_FileName);

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

  res = f_open(&File,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);//创建文件 
  if(res == FR_OK)
  {
    printf("文件创建成功！ \r\n");			
  }
  else
  {
    printf("文件创建失败！ \r\n");
  }		
  res = f_write(&File,DateTime.Buff,sizeof(DateTime.Buff),&Bw);
  if(res == FR_OK)
  {
    printf("文件写入成功！ \r\n");			
  }
  else
  {
    printf("文件写入失败！ \r\n");
  }		
  f_sync(&File);
//  f_close(&File);
#endif  
 
  Logging("System init\r\n");

 // printf("System init---------------------------------------------------------------------------------- \r\n");
//  HAL_Delay(200);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE );
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE );
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE );  

  Logging("UART_ENABLE_IT \r\n");
//  HAL_Delay(200);   
  HAL_UART_Receive_DMA(&huart1,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
  HAL_UART_Receive_DMA(&huart3,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
  HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
  Logging("UART_Receive_DMA \r\n"); 
//  HAL_Delay(200); 
  Read_Lora_ID();
  sprintf(LogBuff,"Lora_ID = %d \r\n",Forward.Lora_ID);
  Logging(LogBuff);
//  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    UserLoop();
    HAL_Delay(5);
    HAL_IWDG_Refresh(&hiwdg); //喂看门狗
//    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//void SD_Write(FIL* fp, const void *buff, UINT btw,	UINT* bw)
//{
//  	uint8_t res=0;
//   // res = f_open(fp,SD_FileName,FA_WRITE);
//   //f_lseek(fp, f_size(fp));
//    res = f_write(fp,WriteBuffer ,strlen(WriteBuffer),bw);
//    f_sync(fp);
//   // f_close(fp);   
//    res= res;
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
