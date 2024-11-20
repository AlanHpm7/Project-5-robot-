//////////////////////
// UART Module Header
/////////////////////


// Header Guard

#ifndef ___UART___
#define ___UART___

#define BAUD_RATE 9600   // configure this for faster comm speed 

// Function Prototypes 
void UART2_Init (void);
void UARTputc(char thisChar);
void UARTputs(char* thisChar);
char UARTgetc (void);
void UARTprintf( char *fmt, ... );
char getcNB(void);


#endif