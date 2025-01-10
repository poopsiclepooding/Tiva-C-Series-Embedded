## Question

*Switch :*
Create a PWM waveform with frequency = 100KHz and variable duty cycle.
The program should begin with d = 50%.
On pressing one switch the duty should be increased by 5% and on pressing other switch it should be decreased by 5%.

*Duration :*
Implement the same but using only 1 switch (SW1 OR SW2) – short press for d increase and long press for decrease.

## Solution

Initlaize GPIO Port F and its Interrupt functionality
```c
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
```

Initialize a PWN Module to work at the specified values
```c
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
```

*Switch :*
Whenever a interrupt comes, it should be handled by GPIO Interrupt Handler. This hanndler changes the duty cycle of PWN depending on which switch was pressed. 
This is handler by dual_switch() function in the code.
```c
void GPIOPortFHandler(void)
{
    dual_switch();                                                  // Call duty cycle change by switch press function

    int wait;
    for(wait = 0; wait <1600*delaycounter/4; wait++){}              // Debouncing Delay of 0.25seconds

    GPIO_PORTF_ICR_R = MASK_BITS;                                   // Clear prior interrupts
    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them
}
```

*Duration :*
Whenever a interrupt comes, it should be handled by GPIO Interrupt Handler. This hanndler changes the duty cycle of PWN depending on which switch was pressed. 
This is handler by single_switch() function in the code.

```c
void GPIOPortFHandler(void)
{
    int wait;
    for(wait = 0; wait <1600*delaycounter/4; wait++){}              //Debounce Delay of 25m

    int i = 0;
    while((GPIO_PORTF_DATA_R & 0x01) == 0){
        i++;
    }
    single_switch(i);                                               // Call duty cycle change by switch press functions

    GPIO_PORTF_ICR_R = MASK_BITS;                                   // Clear prior interrupts
    GPIO_PORTF_IM_R |= MASK_BITS;                                   // Unmask interrupts to enable them
}
```
