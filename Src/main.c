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
char SD_FileName[] = "0:Log7.txt";
uint8_t WriteBuffer[] = "613515412318131354315131515153513512315153131351351351231515316135154123181313543151315151535135123151531313513513512315153161\r\n";

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	FATFS fs;
	FIL file;
	uint8_t res=0;
	UINT Bw;	
  uint16_t FileSize ;
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
 // MX_IWDG_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  printf("System init---------------------------------------------------------------------------------- \r\n");
 // HAL_Delay(200);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE );
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE );
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE );
  
  printf("UART_ENABLE_IT \r\n");
 // HAL_Delay(200); 
  //HAL_TIM_Base_Start_IT(&htim3);
  
 // HAL_UART_Receive_DMA(&huart1,Forward.USART3_Rx_Buff,USART1_RX_MAX_SIZE);
  HAL_UART_Receive_DMA(&huart3,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
  HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
  
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEV_LED_TX_GPIO_Port, DEV_LED_TX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DEV_LED_RX_GPIO_Port, DEV_LED_RX_Pin, GPIO_PIN_SET);
  printf("UART_Receive_DMA \r\n"); 
  HAL_Delay(200); 
 // Read_Lora_ID();
  printf("Lora_ID = %d \r\n",Forward.Lora_ID);
  
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  
	res = SD_init();		//SD卡初始化
	
	if(res == 1)
	{
		printf("SD卡初始化失败! \r\n");		
	}
	else
	{
		printf("SD卡初始化成功！ \r\n");		
	}
  
	res=f_mount(&fs,"0:",1);		//挂载
	printf("%d \r\n",res);	
//	if(test_sd == 0)		//用于测试格式化
	if(res == FR_NO_FILESYSTEM)		//没有文件系统，格式化
	{
//		test_sd =1;				//用于测试格式化
		printf("没有文件系统! \r\n");		
		
	}
  else if(res == FR_OK)
	{
		printf("挂载成功! \r\n");		
	}
	else
	{
		printf("挂载失败! \r\n");
	}	
//  res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);//创建文件
//  f_lseek(&file, f_size(&file));
//  if(res == FR_OK)
//	{
//		printf("打开成功/创建文件成功！ \r\n");		
////		res = f_write(&file,"SD test",sizeof("SD test"),&Bw);		//写数据到SD卡
////		if(res == FR_OK)
////		{
////			printf("文件写入成功！ \r\n");			
////		}
////		else
////		{
////			printf("文件写入失败！ \r\n");
////		}		
//	}
//	else
//	{
//		printf("打开文件失败!\r\n");
//	}	
//  f_close(&file);
//  res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);
//  f_lseek(&file, f_size(&file));
//  res = f_write(&file,"1234567890",sizeof("1234567890"),&Bw);	
//  f_close(&file);
////  
//   res = f_open(&file, SD_FileName,FA_OPEN_EXISTING | FA_READ);//创建文件
//  if(res == FR_OK)
//	{
//		printf("打开成功! \r\n");	
//    res=f_read (&file, readBuffer, sizeof(readBuffer), &br);
//    if(res == FR_OK)
//    {
//      printf("读取成功! \r\n");		
//    }
//    else
//    {
//      printf("读取失败! \r\n");
//    }	    
//	}
//	else
//	{
//		printf("打开失败! \r\n");
//	}	
// 
//  printf("%s\r\n",readBuffer);
  
//  f_close(&file);						//关闭文件		
//	f_mount(NULL,"0:",1);		 //取消挂载

    res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);//创建文件
   // f_lseek(&file, f_size(&file));
    res = f_write(&file,"SD test",sizeof("SD test"),&Bw);
    f_close(&file);
 
   res = f_open(&file, SD_FileName,FA_OPEN_EXISTING | FA_READ);//读取文件
   FileSize = f_size(&file); 
    
    f_close(&file);
    
    res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);//创建文件
   // f_lseek(&file, f_size(&file));
    if(FileSize>50)
    {
      res=f_lseek(&file,FileSize); 
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    

    
    res = f_write(&file,"SDtest  ",sizeof("SDtest  "),&Bw);
    //f_close(&file);    
    HAL_Delay(500);
    if( Forward.USART2_Rx_End_Flag == 1)
    {
    
      USAR3_Transmit();
      printf("UART1_Transmit_Size : %d \r\n",Forward.USART3_Tx_Buff_Size );
      Forward.USART2_Rx_End_Flag = 0;     
      Forward.USART2_Rx_Buff_Size = 0;
      Forward.USART3_Tx_Buff_Size = 0;
      memset(Forward.USART2_Rx_Buff,0,USART2_RX_MAX_SIZE);
      HAL_UART_Receive_DMA(&huart2,Forward.USART2_Rx_Buff,USART2_RX_MAX_SIZE);
    }
    
    if( Forward.USART3_Rx_End_Flag == 1)
    {
    
      USAR2_Transmit();
      printf("UART2_Transmit_Size : %d \r\n",Forward.USART2_Tx_Buff_Size );
      Forward.USART3_Rx_End_Flag = 0;
      Forward.USART3_Rx_Buff_Size = 0;
      Forward.USART2_Tx_Buff_Size = 0;
      memset(Forward.USART3_Rx_Buff,0,USART3_RX_MAX_SIZE);
      HAL_UART_Receive_DMA(&huart3,Forward.USART3_Rx_Buff,USART3_RX_MAX_SIZE);
    }
    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
     HAL_Delay(5);
//     HAL_IWDG_Refresh(&hiwdg); 
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
