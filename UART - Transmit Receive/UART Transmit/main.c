#include<stdint.h>
#include<stdbool.h>
#include "tm4c123gh6pm.h"


void GPIOPortF_Handler(void){
    if(GPIO_PORTF_RIS_R & (1<<4)){
        GPIO_PORTF_ICR_R |= (1<<4);  //Clear Interrupt
        UART1_CTL_R &= ~(0x01);  //Disable UART1
        while(UART1_FR_R & (0x08)){  //Busy mode
            UART1_LCRH_R &= ~(0x10);  //Clear FIFO
            UART1_CTL_R |= (0x80);  //Enable TXE
            UART1_DR_R = (0xF0);  //Data to transmit
            UART1_CTL_R |= (0x01);  //Enable UART1
        }
    }
    if(GPIO_PORTF_RIS_R & (1<<0)){
            GPIO_PORTF_ICR_R |= (1<<0);  //Clear Interrupt
            UART1_CTL_R &= ~(0x01);  //Disable UART1
            while(UART1_FR_R & (0x08)){  //Busy mode
                UART1_LCRH_R &= ~(0x10);  //Clear FIFO
                UART1_CTL_R |= (0x80);  //Enable TXE
                UART1_DR_R = (0xAA);  //Data to transmit
                UART1_CTL_R |= (0x01);  //Enable UART1
            }
        }
}


int main(void){
    //Configure PORTF settings
    SYSCTL_RCGCGPIO_R |= (1<<5);  //Clock for Port F
    while(SYSCTL_PRGPIO_R & (1<<5) == 0);   //Peripherals Ready
    GPIO_PORTF_LOCK_R = 0x4C4F434B;  //Unlock PORTF
    GPIO_PORTF_CR_R |= (0x11);  //Enable changes to PF0 and PF4
    GPIO_PORTF_DIR_R &= ~(0x11);
    GPIO_PORTF_DIR_R |= (0x02);  //Set PF0 and PF4 to input while PF1 to output
    GPIO_PORTF_DEN_R |= (0x13);  //Enable PF0, PF1, PF4
    GPIO_PORTF_PUR_R |= (0x11);  //Enable pull up registers for PF0, PF4

    //Configure UART settings
    SYSCTL_RCGCUART_R |= (1<<1);  //Clock for UART 1
    SYSCTL_RCGCGPIO_R |= (1<<1);  //Clock for Port B. We will use it in Alternate Functionality for UART Tx and Rx
    GPIO_PORTB_AFSEL_R |= (0x03);  //Select alternate functionality for PB0 and PB1
    GPIO_PORTB_PCTL_R |= (1<<4) | (1<<0);  //Set pin control for PB0 and PB1
    GPIO_PORTB_DEN_R |= (0x03);  //Enable PB0 and PB1
           /*UART1 configuration now*/
    UART1_CTL_R &= ~(0x01);  //Disable UART1
    UART1_IBRD_R = 104;
    UART1_FBRD_R = 11;
    UART1_LCRH_R = ((0x3) << 5);  //8-bits of data, 1 stop bit, no parity
    UART1_CC_R = 0x0;  //Clock set to system clock based on calculations above


    //Configure interrupts for PF0 and PF4
    GPIO_PORTF_IM_R &= ~(0x11);  //Disable interrupts
    GPIO_PORTF_IS_R &= ~(0X11);  //Both interrupts are edge sensitive
    GPIO_PORTF_IBE_R &= ~(0x11);  //Only single edge sensitive
    GPIO_PORTF_IEV_R &= ~(0x11);  //Falling edge sensitive
    GPIO_PORTF_ICR_R |= (0x11);  //Clear any interrupts
    GPIO_PORTF_IM_R |= (0x11);  //Enable interrupts

    //Enable NVIC interrupt for GPIO PORTF
    NVIC_EN0_R |= (1<<30);  //IRQ30

    while(1){

    }
}
