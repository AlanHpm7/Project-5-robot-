#include "utility.h"
#include "SysClock.h"
#include "stm32f303xe.h"


void delay_ms(uint32_t msec) {
SysTick->CTRL = 0;                                         // Disable Timer + SysTick interrupt + default AHB/8 clock + clear CountFlag (how?)
SysTick->LOAD = ((SystemCoreClock /8) / 1000UL) * msec;    // Set reload register (like BRR)
SysTick->VAL = 0;                                          // Reset the SysTick counter value
SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;                   // Start SysTick. Use AHBclock/8
while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));      // Wait for timeout
SysTick->CTRL = 0;                                         // Disable SysTick until next time
}
