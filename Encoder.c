/*
 * File: encoder.c
 * Author: Alan Hosseinpour
 * Date: 04/12/2024 (Updated)
 * Description: Functions to manage encoders for mobile robots.
 */

#include "Encoder.h"
/* 
 *                             LOCAL VARIABLES    
 */

uint32_t Global_LeftEncoderPeriod = 0;  // Global period for left encoder
uint32_t Global_RightEncoderPeriod = 0; // Global period for right encoder

/*
 *                             STATIC VARIABLES
 */

static uint32_t leftEncoder[2] = {0, 0}; // Left encoder timestamps [current, previous]
static uint32_t rightEncoder[2] = {0, 0}; // Right encoder timestamps [current, previous]

/* 
 *                            PUBLIC FUNCTIONS
 */

/**
 * @brief Initialize encoders for left (PA0) and right (PA1) wheels.
 * @return None
 */
void Encoder_Init(void) {
    // Enable clock for GPIO port A
    ENABLE_GPIO_CLOCK(A);
    
    // Configure PA0 and PA1 as alternate function mode for encoders
    GPIO_MODER_SET(A, 0, GPIO_MODE_AF);
    GPIO_MODER_SET(A, 1, GPIO_MODE_AF);
    GPIO_PUPDR_SET(A, 0, GPIO_PUPD_NO);
    GPIO_PUPDR_SET(A, 1, GPIO_PUPD_NO);
    GPIO_AFR_SET(A, 0, 1); // PA0 set to TIM2 CH1
    GPIO_AFR_SET(A, 1, 1); // PA1 set to TIM2 CH2
    
    // Enable clock for TIM2
    SET_BITS(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
    SET_BITS(TIM2->PSC, 71UL); // Prescaler set to count in 1us intervals
    CLEAR_BITS(TIM2->CR1, TIM_CR1_DIR); // Set timer to upcounting mode
    
    // Configure TIM2 CH1 for left wheel input capture
    SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC1S_0); // Set CH1 to input capture mode
    SET_BITS(TIM2->CCER, TIM_CCER_CC1E); // Enable input capture for CH1
    CLEAR_BITS(TIM2->CCER, TIM_CCER_CC1P | TIM_CCER_CC1NP); // Detect rising edges
    CLEAR_BITS(TIM2->CCR1, TIM_CCR1_CCR1); // Clear any old data in CCR1
    
    // Configure TIM2 CH2 for right wheel input capture
    SET_BITS(TIM2->CCMR1, TIM_CCMR1_CC2S_0); // Set CH2 to input capture mode
    SET_BITS(TIM2->CCER, TIM_CCER_CC2E); // Enable input capture for CH2
    CLEAR_BITS(TIM2->CCER, TIM_CCER_CC2P | TIM_CCER_CC2NP); // Detect rising edges
    CLEAR_BITS(TIM2->CCR2, TIM_CCR2_CCR2); // Clear any old data in CCR2
    
    // Enable interrupts for CH1 and CH2, and set priority in NVIC
    SET_BITS(TIM2->DIER, TIM_DIER_CC1IE); // Enable CH1 interrupt
    SET_BITS(TIM2->DIER, TIM_DIER_CC2IE); // Enable CH2 interrupt
    NVIC_EnableIRQ(TIM2_IRQn); // Enable TIM2 IRQ in NVIC
    NVIC_SetPriority(TIM2_IRQn, ENCODER_PRIORITY); // Set IRQ priority
    
    // Start TIM2 and initialize input captures
    SET_BITS(TIM2->EGR, TIM_EGR_UG); // Trigger update event
    SET_BITS(TIM2->CR1, TIM_CR1_CEN); // Start TIM2
}

/**
 * @brief TIM2 interrupt handler for left and right wheel encoders.
 * @return None
 */
void TIM2_IRQHandler(void) {
    // Handle left wheel encoder interrupt
    if (IS_BIT_SET(TIM2->SR, TIM_SR_CC1IF)) {
        leftEncoder[1] = leftEncoder[0]; // Update previous timestamp
        leftEncoder[0] = TIM2->CCR1; // Capture current timestamp
    }
    
    // Handle right wheel encoder interrupt
    if (IS_BIT_SET(TIM2->SR, TIM_SR_CC2IF)) {
        rightEncoder[1] = rightEncoder[0]; // Update previous timestamp
        rightEncoder[0] = TIM2->CCR2; // Capture current timestamp
    }
}

/**
 * @brief Calculate the time period for each encoder in microseconds per vane.
 * @return None
 */
void Calculate_Encoder_TimePeriod(void) {
    // Calculate left encoder period and update global variable
    Global_LeftEncoderPeriod = leftEncoder[0] - leftEncoder[1];
    leftEncoder[1] = leftEncoder[0] = 0; // Reset timestamps
    
    // Calculate right encoder period and update global variable
    Global_RightEncoderPeriod = rightEncoder[0] - rightEncoder[1];
    rightEncoder[1] = rightEncoder[0] = 0; // Reset timestamps
}

// Helper function to retrieve the current value of the specified encoder channel
uint32_t getEncoderCHValue(uint8_t enc) {
    if (enc == LEFT_ENC) {
        // Return current value of the left encoder
        return TIM2->CCR1;
    } else if (enc == RIGHT_ENC) {
        // Return current value of the right encoder
        return TIM2->CCR2;
    } else {
        // Invalid input, return 0 as an error value
        return 0;
    }
}