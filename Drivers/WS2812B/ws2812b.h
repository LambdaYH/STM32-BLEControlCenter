#ifndef __WS2812B_H
#define __WS2812B_H

#include "stdint.h"
#include "stm32l1xx_hal.h"
//#include "tim.h"
#define PIXEL_MAX  30
//#define FLASH_REFRESH_RATE      (50)                        //led刷新频率
//#define FLASH_REFRESH_PERIOD    (1000/FLASH_REFRESH_RATE)   //刷新周期
extern uint8_t rBuffer[PIXEL_MAX];
extern uint8_t gBuffer[PIXEL_MAX];
extern uint8_t bBuffer[PIXEL_MAX];
extern TIM_HandleTypeDef htim2;

void WS2812B_Init(void);
void WS2812B_Show(void);
void WS2812B_Close(void);
void WS2812B_GRN_ALL(void);
void WS2812B_BLU_ALL(void);
void WS2812B_RED_ALL(void);
//void    TM7812_Process(void);
//void    TM7812_set_flash(uint8_t flash,uint16_t angle);

//void    TM7812_set_Wheel_1(int16_t speed_L,int16_t speed_R);
//void    tm7812_SetColorPalette(uint8_t g,uint8_t r,uint8_t b); /*测试用函数*/
//void    TM7812B_Test(void);

//void    set_micdir(uint16_t data);

#endif
