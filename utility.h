/*
* File: utility.h
* Author: Alan Hosseinpour
* Date: April 7, 2024 (Uodated)
* Description: Utility macros and functions necesary for the all the codes and Labs .
*/
#include "stm32f303xe.h"
#include <stdio.h>
#include <stdarg.h>
#include "SysClock.h"


#ifndef __UTILITY_H
#define __UTILITY_H


/******************************************************************
 *                             MACROS                             *
 ******************************************************************/

// Bit Manipulation Macros
#define SET_BITS(port, bits) ((port) |= (bits))
#define CLEAR_BITS(port, bits) ((port) &= (~(bits)))
#define FLIP_BITS(port, bits) ((port) ^= (bits))
#define FORCE_BITS(port, mask, value) ((port) = ((port) & (~(mask))) | ((value) & (mask)))

// Logic Bit Testing
#define IS_BIT_SET(port, bits) ((port) & (bits))
#define IS_BIT_CLEAR(port, bits) (!((port) & (bits)))

// Nibble Selection
#define LO_NYBBLE(value) ((value) & 0x0FUL)
#define HI_NYBBLE(value) (((value) >> 4UL) & 0x0FUL)

// Port Naming Macros 
#define GPIO(port) GPIOx(port)
#define GPIOx(port) GPIO##port

// GPIO Register Setting Macros
// GPIO Mode
#define GPIO_MODER_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->MODER), (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define GPIO_MODE_IN 0UL // Input mode
#define GPIO_MODE_OUT 1UL // General purpose output mode
#define GPIO_MODE_AF 2UL // Alternate Function mode
#define GPIO_MODE_ANLG 3UL // Analog mode

// GPIO Output Type
#define GPIO_OTYPER_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->OTYPER), (1UL << (pin)), ((mode) << (pin)))
#define GPIO_OTYPE_PP 0UL // Output push-pull
#define GPIO_OTYPE_OD 1UL // Output open-drain

// GPIO Pull-up/Pull-down
#define GPIO_PUPDR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->PUPDR), (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define GPIO_PUPD_NO 0UL // No pull-up/pull-down
#define GPIO_PUPD_PU 1UL // Pull-up
#define GPIO_PUPD_PD 2UL // Pull-down

// GPIO Alternate Function
#define GPIO_AFR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->AFR[(pin) >> 3]), (15UL << (((pin) & 7UL) * 4)), ((mode) << (((pin) & 7UL) * 4)))

// GPIO Output Speed
#define GPIO_OSPEEDR_SET(port, pin, mode) FORCE_BITS((GPIOx(port)->OSPEEDR), (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define GPIO_OSPEED_LOW 0UL // Low speed
#define GPIO_OSPEED_MED 1UL // Medium speed
#define GPIO_OSPEED_HIGH 3UL // High speed

// GPIO Output Data Register
#define GPIO_ODR_SET(port, pin, state) FORCE_BITS(GPIO(port)->ODR, (1UL << (pin)), ((state) << (pin)))
#define GPIO_ODR_BIT_CLEAR 0UL
#define GPIO_ODR_BIT_SET 1UL

// Enable GPIO Clock
#define ENABLE_GPIO_CLOCK(port) ENABLE_GPIO_CLOCKx(port)
#define ENABLE_GPIO_CLOCKx(port) RCC->AHBENR |= RCC_AHBENR_GPIO##port##EN

/*
  PUBLIC FUNCTIONS 
 */

void delay_ms(uint32_t msec);

#endif // __UTILITY_H