#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

volatile uint32_t dutycycle = 50;
volatile uint32_t dutyupdate = 5;
volatile uint32_t delaycounter = 100;                               // Debouncing delay
volatile uint32_t freq = 100;                                       // This is frequency of PWN in kHz
#define MASK_BITS 0x11                                              //SW1 and SW2 masking

void dual_switch(void);

void GPIOPortF_Init(void)
{
    /* GPIO Port F Configuration */
    SYSCTL_RCGCGPIO_R |= (1<<5);                                    // Enable clock for Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;                                 // Unlock commit register
    GPIO_PORTF_CR_R = 0x1F;                                         // Make Port F pins 0-4 configurable
    GPIO_PORTF_PUR_R = 0x11;                                        // Set pull up registers for Port F pin 0 and 4
    GPIO_PORTF_DEN_R = 0x1F;                                        // Digital Enable Port F pins 0 to 4
    GPIO_PORTF_DIR_R = 0x0E;                                        // Set Port F pins 1,2,3 as output (LED pins) and pins 0,4 as input (Switch)

    /* GPIO PortF Interrupt Configurations */
    GPIO_PORTF_IM_R &= ~MASK_BITS;                                  // Mask interrupt by clearing bits to allow for changing configurations
    GPIO_PORTF_IS_R &= ~MASK_BITS;                                  // Make interrupts edge sensitive
    GPIO_PORTF_IBE_R &= ~MASK_BITS;                                 // Interrupts not triggered by both edges
    GPIO_PORTF_IEV_R &= ~MASK_BITS;                                 // Rising Edge Trigger

    /* Enable interrupt generation in GPIO */
    GPIO_PORTF_ICR_R |= MASK_BITS;                                  // Clear prior interrupts
    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them

    /* Prioritize and enable interrupts in NVIC */
    NVIC_PRI7_R &= 0xFF3FFFFF;                                      // Interrupt priority register 7,  bits 21-23 for port f interrupt 30
    NVIC_EN0_R |= 1 << 30;                                          // Enable Interrupt 30

}

void PWN_Init(void)
{
    /* PWN Configurations */
    SYSCTL_RCGCPWM_R |= (1<<1);                                     // Enable PWM1 clock
    GPIO_PORTF_AFSEL_R |= (1<<1);                                   // Enable alternate function of GPIO pins
    GPIO_PORTF_PCTL_R |= 0x50;                                      // Make PF1 as PWM output
    PWM1_2_CTL_R |= 0x00;                                           // Disable PWM2 while configuring and select down count mode
    PWM1_2_GENA_R = 0x8C;                                           // Set PWM2A to produce a symmetric down-counting PWM signal
    PWM1_2_GENB_R = 0x8C;                                           // Set PWM2B to produce a symmetric down-counting PWM signal
    PWM1_2_LOAD_R = 16000/freq - 1;                                 // Set LOAD register for PWN of frequency as freq
    PWM1_2_CMPA_R = (PWM1_2_LOAD_R * dutycycle)/100;                // Set CMP register for 50% duty cycle
    PWM1_2_CTL_R |= 0x01;                                           //Enable generator 3 counter
    PWM1_ENABLE_R |= 0x20;                                          // Enable PWM1 channel 6 Output
}

void main(void)

{
    GPIOPortF_Init();
    PWN_Init();
    while(1){
        //do nothing
    }
}

void GPIOPortFHandler(void)
{
    dual_switch();                                                  // Call duty cycle change by switch press function

    int wait;
    for(wait = 0; wait <1600*delaycounter/4; wait++){}              // Debouncing Delay of 0.25seconds

    GPIO_PORTF_ICR_R = MASK_BITS;                                   // Clear prior interrupts
    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them
}

void dual_switch(void)
{
    GPIO_PORTF_IM_R &= ~MASK_BITS;                                  // Mask interrupts to disable them till current interrupt is handled

    if (GPIO_PORTF_RIS_R & 0x01)                                    // SW 1 is pressed
    {
        if (dutycycle > dutyupdate){
            dutycycle = dutycycle - dutyupdate;
        }
        if (dutycycle <= 5){
            dutycycle = 5;
        }
    }

    if(GPIO_PORTF_RIS_R & 0x10)                                     // SW 2 is pressed
    {
        if (dutycycle < 100 - dutyupdate){
            dutycycle = dutycycle + dutyupdate;
        }
        if (dutycycle >= 90){
            dutycycle = 90;
        }
    }

    if (GPIO_PORTF_RIS_R & 0x11)                                    // Both SW1 and SW2 pressed
    {
        dutycycle = dutycycle;
    }

    PWM1_2_CMPA_R = (PWM1_2_LOAD_R * dutycycle)/100;                // Update the CMP, hence updating the duty cycle
}





