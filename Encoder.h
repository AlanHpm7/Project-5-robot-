/*
 * File: Encoder.h
 * Author: Alan Hosseinpour
 * Date: 04/12/2024 (updated)
 * Description: Header file for encoder functions used in mobile robots.
 */

#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f303xe.h" 
#include "utility.h" 

// Definitions for encoder ports and timers
#define ENCODER_PORT A // Port A for encoders

#define ENCODER_TIMER TIM2          // Timer 2 for encoders
#define ENCODER_TIMER_INT TIM2_IRQn // Interrupt number for Timer 2

// Definitions for left and right encoders
#define LEFT_ENCODER_CH   CCR1 // Left encoder channel (Capture/Compare 1)
#define LEFT_ENCODER_PIN  0    // PA0 pin for left encoder

#define RIGHT_ENCODER_CH   CCR2 // Right encoder channel (Capture/Compare 2)
#define RIGHT_ENCODER_PIN  1    // PA1 pin for right encoder

// Priority for encoder interrupts
#define ENCODER_PRIORITY 9 // Interrupt priority level

// Constants for specifying left and right encoders
#define LEFT_ENC  0 // Left encoder identifier
#define RIGHT_ENC 1 // Right encoder identifier

// Helper Function for Debugging
uint32_t getEncoderCHValue( uint8_t enc );

// Function declarations
void Encoder_Init(void); // Initialize encoders
void TIM2_IRQHandler(void); // Handle encoder interrupts
void Calculate_Encoder_TimePeriod(void); // Calculate time period for encoders
void calculateAndDisplayMotorSpeed(void); // Calculate and display motor speed

// External global variables
extern uint32_t Global_LeftEncoderPeriod; // Left encoder period (us per vane)
extern uint32_t Global_RightEncoderPeriod; // Right encoder period (us per vane)

#endif // __ENCODER_H