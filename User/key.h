#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define	KEY_ON	 1		//按键按下
#define	KEY_OFF	 0		//按键放开

//extern uint8_t keyCode;


//#define KEY1_GPIO_CLK               RCC_APB2Periph_GPIOB
//#define KEY1_GPIO_PORT              GPIOB
//#define KEY1_PIN                    GPIO_Pin_12


typedef enum
{
  KEY_1 = 0,
  KEY_COUNT,
} key_item_t;

typedef struct{
	uint16_t KeyStatus;
	uint16_t lastKeyStatus;
	uint16_t keyPressedJump;
	uint16_t keyReleaseJump;
	uint16_t longPressed;
  uint16_t longPressedJump;
  uint16_t longReleaseJump;
	uint16_t timer;
}key_t;


extern key_t userKeyList[KEY_COUNT];

void Key_Init(void);
void Key_Process(void);

//extern TaskHandle_t xHandleTaskKey;
//void vTaskKey(void *pvParameters);

#endif /* __KEY_H */
