/*
Author: Alan Hosseinpour
File: UART.c
Description: Implementation file for UART communication on the STM32F303XE microcontroller. 
             Initializes USART2, provides functions for sending and receiving characters, and includes a printf function.

Date: 02/11/2024
*/


#include "stm32f303xe.h"
#include <stdio.h>
#include "UART.h"
#include <stdarg.h>
#include "SysClock.h"

void UART2_Init(void)
{	
////
//Part 1: Clock resource config
////

// enable USART2 Clock to  EnAble the subsystem
RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;

//Select USART2 clock source 
//  00: PCLK ( Peripheral crystal clock, NC)
//  01: SYSTEM CLOCK (sysclk) << this is hwat we want
//  10: HSI16 clock soruce 
//  11: LSE clock source
//  We want SYSCLK
	RCC -> CFGR3 &= ~RCC_CFGR3_USART2SW_1;  // clear the MSB
	RCC -> CFGR3 |= RCC_CFGR3_USART2SW_0;   // set the LSB

////
// Part II : GPIO config to neable PA2 & PA3 for UART2 afS 
////

// Enable the GPIO clock to enable it. (Where is USART2 located?)
// We will use Port A (most commonly used)
RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;  //same line as that in LED
	
	//Then, Configure the desired pins - we will use PA2 and PA3 (common choice)
	//    PA.2 => USART2 TX (on AF7)
	//    PA.3 => USART2 RX (on AF7)
	// Thus, we need to set up 1) Alternative Function, 2) Speed (Low Speed?), 3) Push-Pull Mode, and 4) Pull-Up
	
// 2. Set the MODER for PA2 and PA3 to AF (ob10)
GPIOA-> MODER &=  ~(0x03UL << (2*2));  // clear out  GPIOA pin 2 Moe bits for selecting AF
GPIOA-> MODER &=  ~(0x03UL << (2*3));  // Clear out GPIOA pin 3  Mode bits for selecting AF

GPIOA->MODER |= (0x02UL << (2*2)); // Set GPIOA Pin 2 Mode bits to '10' for AF
GPIOA->MODER |= (0x02UL << (2*3)); // Set GPIOA Pin 3 Mode bits to '10' for AF
// 3. Select USART2 as the alternative function 
// Hint: AFR []
GPIOA -> AFR[0] |= (0x07UL <<  (4*2));  // Clear  PA2 to AF7 selection 0x0F = 0b00001111
GPIOA -> AFR[0] |= (0x07UL <<  (4*3));  // Clear  PA3 to AF7 selcetion

//GPIOA -> AFR[0] &=  ~(0x0FUL << (2*4));  // Set PA2 to AF7 
//GPIOA -> AFR[0] &=  ~(0x0FUL << (3*4));  // Set PA3 to AF7
// AFR[0] is AFRL in ref manual
// AFR[1] is AFRL in ref manual


// 4. sET THE Output speed (OSPEED) register to low speed 
// 00 - low, - 01 MED, 10 - Fast, 11 - High
// Hint: OSPEEDR
GPIOA ->OSPEEDR &=  ~(0x03UL << (2*2));  // Low speed for PA2
GPIOA ->OSPEEDR &=  ~(0x03UL << (2*3));  // Low speed for PA3

// 5. Set PUPDR of PA2 and PA3 to no pull 
//  00 - No pull, 01 - Pull up , 10 - pull DOWN, 11 - reserved 
GPIOA ->PUPDR &=  ~(0x03UL << (2*2));  // No Pull for PA2
GPIOA ->PUPDR &=  ~(0x03UL << (2*3));  // No pull for PA3
// 6. Set OTYPER to pish_pull  for PA2 and PA3 
// 0 - PP, 1 - OD
GPIOA ->OTYPER &=  ~(0x1UL << 2);  // PP for PA3
GPIOA ->OTYPER &=  ~(0x1UL << 3);  // PP for PA3

//Done for part II 



////
//Part III: CONFIGURE  the UART2 susbystem for 9600 baud comm
////

// 1. Disable UART2 (write 0 into UE of USART2->CR1)
USART2 -> CR1 &= ~USART_CR1_UE; // REF MANUAL + STM32F303XE.h

// 2. Set word length (write 0 to both M1 and M0 OF usart2->CR1)
USART2 -> CR1 &= ~USART_CR1_M;

// 3. SET OVERSMAPLING RATE TO 16X( write 0 into OVER8 of USART2->CR1)
USART2 -> CR1 &= ~USART_CR1_OVER8;

// 4. Set stop bit Config to 1 stop bit (write 00 int STOP bits of USART2 -> CR1) 
USART2 -> CR2 &= ~USART_CR2_STOP;

// 5. Set the BAUD rate register 
//     BRR_value = systemclock / BaudRate
//     SystemCoreClock (72000000) defined in SysClock.h
//     BAUDRate already defined in UART.h
//     therefore ...
///    Write SystemCoreClock / BAUD_Rate into USART2->BRR
USART2 -> BRR = SystemCoreClock / BAUD_RATE;

// 6. Turn on Transmitter and receiver Blocks by writing 1 into RE and TE of USART2->CR1
USART2 ->CR1 |= USART_CR1_RE; // enabling Receiver Block
USART2 ->CR1 |= USART_CR1_TE; // enabling transmitter Block


// 7. Re-enable USART2 by writing 1 into UE or USART2 -> CR1 
USART2 -> CR1 |= USART_CR1_UE; // REF MANUAL + STM32F303XE.h\

// 8. WAit for REACK and TEACK to turn to 1 on USART2->ISR
while( (USART2->ISR & USART_ISR_TXE) == 0);  // wait till transmitt
//while( (USART2->ISR & USART_ISR_REACK) == 0);
GPIOA->ODR &= ~(1UL << 5);
//FINALLY .... DONE 

}

/*
 * Function: UARTputc
 * -------------------
 * Sends a single character over UART.
 * Waits until the Transmit Data Register Empty (TXE) flag is set before sending.
 * Uses USART2 for communication.
 */
void UARTputc(char thisChar)
{
    while ((USART2->ISR & USART_ISR_TXE) == 0);  // Wait until TDR is empty
    USART2->TDR = (uint8_t)thisChar;
}

/*
 * Function: UARTputs
 * -------------------
 * Sends a null-terminated string over UART using UARTputc for each character.
 * Uses USART2 for communication.
 */
void UARTputs(char *thisChar)
{
    while (*thisChar)
    {
        UARTputc(*thisChar);
        thisChar++;
    }
}

/*
 * Function: UARTgetc
 * -------------------
 * Receives a single character over UART.
 * Waits until the Receive Data Register Not Empty (RXNE) flag is set before reading.
 * Uses USART2 for communication.
 */
char UARTgetc(void)
{
	while((USART2->ISR & USART_ISR_RXNE) == 0);
	return(char)(USART2->RDR);
}



/*
 * Macro: MAX_UART_BUFSIZ
 * ----------------------
 * Defines the maximum buffer size used for formatting strings in UARTprintf.
 */
#define MAX_UART_BUFSIZ 120

/*
 * Function: UARTprintf
 * ---------------------
 * Prints a formatted string over UART using variable arguments.
 * Uses USART2 for communication.
 */
void UARTprintf(char *fmt, ...)
{
    va_list args;

    // Need to guarantee that buffer is big enough for any string
    char buffer[MAX_UART_BUFSIZ];

    va_start(args, fmt);
    vsnprintf(buffer, MAX_UART_BUFSIZ, fmt, args);
    va_end(args);

    // Send the formatted string over UART
    UARTputs(buffer);
	}

	/*
 * Function: getcNB
 * -----------------
 * Non-blocking function to receive a single character over UART.
 * Returns '\0' if no character is available.
 * Uses USART2 for communication.
 */
	char getcNB(void)
{
    if(!((USART2->ISR & USART_ISR_RXNE) == 0)){


        return(char)(USART2->RDR);
    }

    return('\0'); // when nothing is inputted on the keyboard, then the code continues to run. 

 }


// Above and beyond 