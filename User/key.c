/**
  ******************************************************************************
  * @file    key.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   按键应用函数接口
  ******************************************************************************
  */

#include "key.h"

typedef struct{
	uint32_t GPIO_CLK;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}GPIOPins_t;

key_t userKeyList[KEY_COUNT];
uint8_t keyCode = 0;

GPIOPins_t Keys_GPIO[KEY_COUNT] = 
{
	{0, KEY1_GPIO_Port, KEY1_Pin},
};

/**
  * @brief  按键初始化
  * @param  None
  * @retval None
  */
void Key_Init(void)
{		
//	for(int i = 0; i < KEY_COUNT; i++){
//		GPIO_QuickInit(Keys_GPIO[i].GPIO_CLK, Keys_GPIO[i].GPIOx, Keys_GPIO[i].GPIO_Pin, GPIO_Mode_IPU);
//	}

}

void Key_Process(void)
{
	keyCode = 0;
	for(int i = 0; i < KEY_COUNT; i++)
  {
		userKeyList[i].keyReleaseJump = 0;
		userKeyList[i].keyPressedJump = 0;
    userKeyList[i].longPressedJump = 0;
    userKeyList[i].longReleaseJump = 0;
		
		userKeyList[i].KeyStatus = !HAL_GPIO_ReadPin(Keys_GPIO[i].GPIOx, Keys_GPIO[i].GPIO_Pin);
		
		if (userKeyList[i].KeyStatus != userKeyList[i].lastKeyStatus)
    {
			if(userKeyList[i].KeyStatus)
      {
				keyCode = 1 << i;
				userKeyList[i].keyPressedJump = userKeyList[i].KeyStatus;
			}
			else
      {
        if(userKeyList[i].longPressed)
        {
          userKeyList[i].longReleaseJump = userKeyList[i].lastKeyStatus;
        }
        else
        {
          userKeyList[i].keyReleaseJump = userKeyList[i].lastKeyStatus;
        }
				userKeyList[i].longPressed = 0;
				userKeyList[i].timer = 0;
			}
		}
    else
    {
      if(userKeyList[i].KeyStatus && (userKeyList[i].longPressed == 0))
      {
        userKeyList[i].timer++;
        if(userKeyList[i].timer > 100)
        {
          userKeyList[i].longPressed = userKeyList[i].KeyStatus;
          userKeyList[i].longPressedJump = userKeyList[i].KeyStatus;
          userKeyList[i].timer = 0;
        }
      }
    }
		
		
		
		userKeyList[i].lastKeyStatus = userKeyList[i].KeyStatus;
	}
}




