#include<stdint.h>
#include<stdbool.h>
#include "tm4c123gh6pm.h"

void UART1_Init(void);
char UART1_ReadChar(void);
void LED_Init(void);
void LED_Value(char receivedChar);

void UART1_Init(void){
    SYSCTL_RCGCUART_R |= (1 << 1);
    SYSCTL_RCGCGPIO_R |= (1 << 1);
    while ((SYSCTL_PRGPIO_R & (1 << 1)) == 0);
    GPIO_PORTB_AFSEL_R |= (1 << 0) | (1 << 1);
    GPIO_PORTB_PCTL_R |= (1 << 0) | (1 << 4);
    GPIO_PORTB_DEN_R |= (1 << 0) | (1 << 1);
    UART1_CTL_R &= ~(0x01);
    UART1_IBRD_R = 104;
    UART1_FBRD_R = 11;
    UART1_LCRH_R = (0x3 << 5);
    UART1_CC_R = 0x0;
    UART1_CTL_R |= (0x01 | 0x200);
}

char UART1_ReadChar(void) {
    while ((UART1_FR_R & 0x10) != 0);
    return UART1_DR_R & 0xFF;
}

void LED_Init(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 5);
    while ((SYSCTL_PRGPIO_R & (1 << 5)) == 0);
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
}

void LED_Control(char receivedChar) {
    GPIO_PORTF_DATA_R &= ~(0x0E);

    if (receivedChar == 0xAA) {
        GPIO_PORTF_DATA_R |= (1 << 3);
    } else if (receivedChar == 0xF0) {
        GPIO_PORTF_DATA_R |= (1 << 2);
    } else {
        GPIO_PORTF_DATA_R |= (1 << 1);
    }
}

int main(void){
    UART1_Init();
    LED_Init();
    while (1) {
            received = UART1_ReadChar();
            LED_Control(received);
        }
}
