#ifndef LED_H
#define LED_H


#include "stm32f303xe.h"

void LED_Init(void);
void LED_Flash(uint32_t number_of_seconds);
void LED_Toggle(void);



#endif