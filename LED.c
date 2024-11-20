/*
Author: Alan Hosseinpour
Date: 03/22/2024
File: LED.c
Description: Implementation file for interfacing with an LED using an STM32F303XE microcontroller.
Provides functions to initialize the LED, toggle the LED state, and flash the LED for a given duration.
*/

#include "stm32f303xe.h"
#include "LED.h"
#include "utility.h"

/*
 * Function: LED_Init
 * Description: Initialize the LED by setting up the necessary GPIO pins.
 * Inputs: None
 * Returns: None
 */
void LED_Init(void) {
    // Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set GPIOA PA5 to output mode
    GPIOA->MODER &= ~(3UL << (2 * 5)); // Clear the 2 bits for PA5
    GPIOA->MODER |= (1UL << (2 * 5)); // Set PA5 to output mode (01)

    // Set GPIOA PA5 output type to push-pull (logic 0)
    GPIOA->OTYPER &= ~(1UL << 5);

    // Set GPIOA PA5 pull-up/pull-down configuration to no pull
    GPIOA->PUPDR &= ~(3UL << (2 * 5));

    // Initialize LED (PA5) to ON state
    GPIOA->ODR |= (1UL << 5);
}

/*
 * Function: LED_Toggle
 * Description: Toggle the LED state (ON or OFF).
 * Inputs: None
 * Returns: None
 */
void LED_Toggle(void) {
    // Toggle the state of the LED connected to PA5 using XOR
    GPIOA->ODR ^= (1UL << 5);
}

/*
 * Function: LED_Flash
 * Description: Flash the LED by turning it ON for a specified number of seconds.
 * Inputs:
 *   - number_of_seconds: Duration (in seconds) to keep the LED ON.
 * Returns: None
 */
void LED_Flash(uint32_t number_of_seconds) {
    // Delay for the specified duration
    delay_ms(number_of_seconds * 1000);
    // Toggle the LED state to OFF
    LED_Toggle();
}