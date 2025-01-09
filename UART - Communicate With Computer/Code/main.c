#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "printf.h"
#include "trace.h"

void GPIOPortA_Init(void){
    /* GPIO Port A Configuration */
    SYSCTL_RCGCGPIO_R |= (1<<0); // Enable clock for Port A
    GPIO_PORTA_DEN_R = 0x03; // Digital Enable Port B pins 0,1
    GPIO_PORTA_DIR_R = 0x02; // Set Port A pin 0 to input and pin 1 to output
}

void GPIOPortF_Init(void){
    /* GPIO Port F Configuration */
    SYSCTL_RCGCGPIO_R |= (1<<5);                                    // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;                                 // Unlock commit register
    GPIO_PORTF_CR_R = 0x1F;                                         // Make Port F pins 0-4 configurable
    GPIO_PORTF_PUR_R = 0x11;                                        // Set pull up registers for Port F pin 0 and 4
    GPIO_PORTF_DEN_R = 0x1F;                                        // Digital Enable Port F pins 0 to 4
    GPIO_PORTF_DIR_R = 0x0E;                                        // Set Port F pins 1,2,3 as output (LED pins) and pins 0,4 as input (Switch)
}

void delay(int delay){
    int wait;
    for(wait = 0; wait < delay; wait++){}
}

void main(void){

    GPIOPortA_Init(); // Initialize PORT A, it is used to communicate with monitor
    GPIOPortF_Init(); // Initialize PORT F, it is used to blink the LEDs
    uart_init(); // Initialize UART 0
    printf("Enter 'r' or 'b' or 'g': \n");
    while (1){
        // do nothing
    }
}










//#define MASK_BITS 0x11                                              //SW1 and SW2 masking

//void single_switch(int);
//
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
//{
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





