#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

volatile bool buttonpressed = false;                                // This keeps track of if switch pressed
volatile uint32_t delaycounter = 10;                                // This is delay for de-bouncing

void PortF_INIT(void)
{
    //---------------------- SET UP GPIO PORT F DATA REGISTERS -------------------------------
    SYSCTL_RCGCGPIO_R |= 0x20;                                      // This enables clock for the GPIO Port F Register
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {};                       // This waits till clock is enabled
    GPIO_PORTF_LOCK_R = 0x4C4F434B;                                 // This unlocks Port F for working
    GPIO_PORTF_CR_R = 0xFF;                                         // This allows changing control registers of specific GPIO pins
    GPIO_PORTF_DIR_R = 0x0E;                                        // This sets input or output for each pin
    GPIO_PORTF_PUR_R = 0x11;                                        // This sets pull up resistors for each pin
    GPIO_PORTF_DEN_R = 0x1F;                                        // This enables each pin

    //---------------------- SET UP GPIO PORT F INTERRUPT REGISTERS -------------------------------
    GPIO_PORTF_IM_R &= ~0x11;                                       // This sets mask so that control registers for interrupts can be changed
    GPIO_PORTF_IS_R &= ~0x11;                                       // This clears IS PF0, PF4 for enabling edge detection
    GPIO_PORTF_IEV_R |= 0x11;                                       // This sets IEV PF0, PF4 for rising edge detection
    GPIO_PORTF_ICR_R = 0x11;                                        // This sets ICR PF0, PF4 to clear interrupts for it
    GPIO_PORTF_IM_R |= 0x11;                                        // This un-masks the interrupts again

    NVIC_EN0_R = (1 << 30);                                         // This enables Port F interrupts from NVIC
}

void GPIOPortFHandler(void){
    if (GPIO_PORTF_RIS_R & 0x01) {                                  // This checks if interrupt came from PF0
        GPIO_PORTF_ICR_R = 0x01;                                    // This clears interrupt flag for PF0
        buttonpressed = true;                                       // This toggles data in Port F Pin 3
    }
    if (GPIO_PORTF_RIS_R & 0x10) {                                  // This checks if interrupt came from PF4
        GPIO_PORTF_ICR_R = 0x10;                                    // This clears interrupt flag for PF4
        buttonpressed = true;                                       // This toggles data in Port F Pin 3
    }
}

void Delay(uint32_t count) {
    while (count--) {
        volatile uint32_t i;
        for (i = 0; i < 1000; i++) {
            // Empty loop for delay
        }
    }
}
int main(void)
{
    PortF_INIT();                                                   // This calls Port F initialization
    while (1){
        if (buttonpressed){
            Delay(delaycounter);
            GPIO_PORTF_DATA_R ^= 0x02;
            buttonpressed = false;
        }
    }
	return 0;
}



