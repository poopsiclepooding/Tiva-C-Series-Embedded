#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "printf.h"
#include "trace.h"

/* SysTick Registers Define */
#define STCTRL *((volatile long *) 0xE000E010)
#define STRELOAD *((volatile long*) 0xE000E014)
#define STCURRENT *((volatile long *) 0xE000E018)

/* SysTick Control Registers Define */
#define ENABLE (1 << 0) // Enable SysTick Counter
#define CLKINT (1 << 2) // System CLK is used by writing 1 here
#define STINT  (1 << 1) // SysTick Interrupt is enabled
#define CLOCK_MHZ 16 // System CLK Speed

/* UART Port A Pins Define */
#define UART_Tx_Pin 0x02
#define UART_Rx_Pin 0x01

/* Global variables used for tx */
volatile uint_8 tx_data = 0;
volatile uint_8 tx_index = 0;
volatile uint_8 tx_parity = 0;
volatile bool tx_busy = false;
volatile bool partity = false;

/* Baud Rate for the bit banging communication */
#define BAUDRATE 9600

void UART_TxBitBang_Init(){
    /* GPIO Port A Configuration */
    SYSCTL_RCGCGPIO_R |= (1<<0); // Enable clock for Port A
    GPIO_PORTA_DEN_R |= 0x02; // Digital Enable Port A pins 0
    GPIO_PORTA_DIR_R |= 0x02 ; // Set Port A pin 1 to output
    GPIO_PORTA_DATA_R |= 0x02; // Set Porta A pin 1 (output) to idle state high
}

void UART_RxBitBang_Init(){
    /* GPIO Port A Configuration */
    SYSCTL_RCGCGPIO_R |= (1<<0); // Enable clock for Port A
    GPIO_PORTA_DEN_R |= 0x01; // Digital Enable Port A pins 1
    GPIO_PORTA_DIR_R &= ~0x01 ; // Set Port A pin 0 to input
}

void SystTick_Init(){
    if (parity==false){
        STRELOAD = 0x411B; // Calculated from baud rate, (1/baud_rate)*no_of_bits*clk_freq, (1/9600)*10*16000000
    } else {
        STRELOAD = 0x479D; // Calculated from baud rate, (1/baud_rate)*no_of_bits*clk_freq, (1/9600)*11*16000000
    }
    STCTRL |= (CLKINT | STINT);
}

void UART_Transmit(uint8_t data){
    while(tx_busy){
        // do nothing
    }
    tx_data = data; // Set tx_data to current data
    tx_index = 0; // Reset index to 0
    tx_busy = true; // Set busy flag to true

    if (parity==false){
        STRELOAD = 0x411B; // Calculated from baud rate, (1/baud_rate)*no_of_bits*clk_freq, (1/9600)*10*16000000
    } else {
        STRELOAD = 0x479D; // Calculated from baud rate, (1/baud_rate)*no_of_bits*clk_freq, (1/9600)*11*16000000
    }
    STCTRL |= ENABLE; // Enable SysTick timer to send data via interrupts at baud rate
}


void delay(int delay){
    int wait;
    for(wait = 0; wait < delay; wait++){}
}

void main(void){

    UART_RxBitBang_Init(); // Initialize PORT A Pin 0 UART Rx
    UART_TxBitBang_Init(); // Initialize Port A Pin 1 UART Tx
    while (1){
        UART_Transmit('A');
        UART_Transmit('B');
        UART_Transmit('C');
    }
}

void SysTickHandler(void){

    /* Transmit the start bit */
    if (tx_index==0){
        GPIO_PORTA_DATA_R &= ~UART_Tx_Pin; // Send UART Start Bit (1)
        tx_index += 1;
    }

    /* Transmit the 8 bits of data in UART */
    while ((tx_index > 0) & (tx_index < 8)){
        /* If tx_data has 1 at tx_index then set GPIO Port A Pin 1
         * If tx_data has 0 at tx_index then clear GPIO Port A Pin 1 */
        if ((tx_data >> tx_index) & 1){
            GPIO_PORTA_DATA_R |= UART_Tx_Pin;
            tx_parity ^= 0x01;
        } else {
            GPIO_PORTA_DATA_R &= ~UART_Tx_Pin;
        }

        /* Increase tx_index to go to next bit in tx_data */
        tx_index += 1;
    }

    /* Send parity bit if true */
    if ((parity==true) & (tx_index==8)){
        if (tx_parity == 1){
            GPIO_PORTA_DATA_R |= UART_Tx_Pin;
        } else {
            GPIO_PORTA_DATA_R &= ~UART_Tx_Pin;
        }
        tx_index += 1;
    }

    /* Transmit stop bit as the 10th bit(if no parity) or 11th bit(if parity)in UART */
    if (((parity==false) & (tx_index==8)) | ((parity==true) & (tx_index==9))){
        GPIO_PORTA_DATA_R |= UART_Tx_Pin;
        tx_index = 0;
        tx_busy = false
    }

    /* Disable SysTick*/
    STCTRL &= ~ENABLE;
}








//#define MASK_BITS 0x11                                              //SW1 and SW2 masking

//void single_switch(int);

//void GPIOPortA_Init(void){
//    /* GPIO Port A Configuration */
//    SYSCTL_RCGCGPIO_R |= (1<<0); // Enable clock for Port A
//    GPIO_PORTA_DEN_R = 0x03; // Digital Enable Port A pins 0,1
//    GPIO_PORTA_DIR_R = 0x02; // Set Port A pin 0 to input and pin 1 to output
//    GPIO_PORTA_DATA_R |= 0x02; // Set Porta A pin 1 (output) to idle state high
//}

//void GPIOPortF_Init(void)
//{
//    /* GPIO Port F Configuration */
//    SYSCTL_RCGCGPIO_R |= (1<<5);                                    // Enable clock for Port F
//    GPIO_PORTF_LOCK_R = 0x4C4F434B;                                 // Unlock commit register
//    GPIO_PORTF_CR_R = 0x1F;                                         // Make Port F pins 0-4 configurable
//    GPIO_PORTF_PUR_R = 0x11;                                        // Set pull up registers for Port F pin 0 and 4
//    GPIO_PORTF_DEN_R = 0x1F;                                        // Digital Enable Port F pins 0 to 4
//    GPIO_PORTF_DIR_R = 0x0E;                                        // Set Port F pins 1,2,3 as output (LED pins) and pins 0,4 as input (Switch)
//
//    /* GPIO PortF Interrupt Configurations */
//    GPIO_PORTF_IM_R &= ~MASK_BITS;                                  // Mask interrupt by clearing bits to allow for changing configurations
//    GPIO_PORTF_IS_R &= ~MASK_BITS;                                  // Make interrupts edge sensitive
//    GPIO_PORTF_IBE_R &= ~MASK_BITS;                                 // Interrupts not triggered by both edges
//    GPIO_PORTF_IEV_R &= ~MASK_BITS;                                 // Rising Edge Trigger
//
//    /* Enable interrupt generation in GPIO */
//    GPIO_PORTF_ICR_R |= MASK_BITS;                                  // Clear prior interrupts
//    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them
//
//    /* Prioritize and enable interrupts in NVIC */
//    NVIC_PRI7_R &= 0xFF3FFFFF;                                      // Interrupt priority register 7,  bits 21-23 for port f interrupt 30
//    NVIC_EN0_R |= 1 << 30;                                          // Enable Interrupt 30
//
//}
//
//void GPIOPortFHandler(void)
//{
//    int wait;
//    for(wait = 0; wait <1600/4; wait++){}              //Debounce Delay of 25m
//
//    int i = 0;
//    while((GPIO_PORTF_DATA_R & 0x01) == 0){
//        i++;
//    }
//    single_switch(i);                                               // Call duty cycle change by switch press functions
//
//    GPIO_PORTF_ICR_R = MASK_BITS;                                   // Clear prior interrupts
//    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them
//}
//
//void single_switch(int i)
/={
//    GPIO_PORTF_IM_R &= ~MASK_BITS;                                  // Mask interrupts to disable them till current interrupt is handled
//
//    if (GPIO_PORTF_RIS_R & 0x01)                                    // SW 2 is pressed
//    {
//        if (i > 90000){                                             // If long press then reduce duty cycle
//            if (dutycycle > dutyupdate){
//                dutycycle = dutycycle - dutyupdate;
//            } else {
//                dutycycle = dutycycle;
//            }
//        }
//        if (i > 0 & i < 90000){                                     // If short press then reduce duty cycle
//            if (dutycycle < 100 - dutyupdate){
//                dutycycle = dutycycle + dutyupdate;
//            } else {
//                dutycycle = dutycycle;
//            }
//        }
//    }
//
//    if(GPIO_PORTF_RIS_R & 0x10)                                     // SW 2 is pressed
//    {
//        // do nothing
//    }
//
//    PWM1_2_CMPA_R = (PWM1_2_LOAD_R * dutycycle)/100;                // Update the CMP, hence updating the duty cycle
//}





