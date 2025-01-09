# Group12_Lab08

## Question
1. Program your micro-controller to serially communicate with your laptop/computer.

2. If a 'R' is sent from your computer to the board, turn the 'RED' LED on. If 'B' is received by the micro-controller turn BLUE LED on, for 'G' tuen GREEN LED on.

3. If any other character is received turn the LEDs off.

4. The character received by the character should also be sent back by the micro-controller to the computer.



## Solution


In tm4c123gh6pm_startup_ccs.c file define handler as extern. Also scroll down to replace IntDefaultHandler with UART0Handler in the place of UART0 in vector table.
```c
extern void UART0Handler(void);
```

Set appropriate values to all registers of GPIO pins
```c
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
```

Set appropriate values to all registers related to UART
```c
void uart_init(void){

    /* UART Configurations */

    // Enable UART0 clock
    SYSCTL_RCGCUART_R |= (1<<0);
    // Enable alternate function of GPIO pins
    GPIO_PORTA_AFSEL_R |= (1<<1) | (1<<0);
    // Make PA0 and PA1 UART Rx and Tx pins
    GPIO_PORTA_PCTL_R |= 0x11;

    // Disable UART0 to change configurations
    UART0_CTL_R &= ~0x01;

    // Set baud-rate divisor for getting appropriate baud-rate
    UART0_IBRD_R = UART_BAUDRATE_INT;
    UART0_FBRD_R = UART_BAUDRATE_FRACTIONAL;

    /* Use settings (data word length 8 bits, no parity bits, 1 stop bit, no FIFO)
     * [ Start (1 bit) | Data (8 bits) | Stop (1 bit) ] */
    UART0_LCRH_R = 0x60;

    // Use default system clock (16Mhz)
    UART0_CC_R = 0x00;

    // Enable UART0
    UART0_CTL_R |= 0x01;

    /* Enable Interrupts of UART */
    // Interrupt priority register 7,  bits 13:15 for UART0 interrupt, Priority Level set 3
    NVIC_PRI1_R &= 0xFFFFF3FF;
    // 5th Bit of NVIC corresponds to UART0 interrupt
    NVIC_EN0_R |= 1 << 5;
    // Set bit in mask register to enable interrupt
    uart_enable_tx_interrupt();
    uart_enable_rx_interrupt();
    // Clear interrupt in case set at start
    uart_clear_tx_interrupt();
    uart_clear_rx_interrupt();
}
```

A printf() functionality is added for printing "Enter 'r' or 'b' or 'g': \n" at start. This is done by implementing _putchar() function and using this repo href[https://github.com/mpaland/printf.git][printf]. 
```c
void _putchar(char c){

    // Block transmission if ring buffer full
    while(ring_buffer_full(&tx_buffer)){}

    // Disable Interrupt to create a mutex
    uart_disable_tx_interrupt();

    const bool empty = ring_buffer_empty(&tx_buffer); // Check if tx ongoing

    // Add carriage return before new line
    if(c == '\n'){
        ring_buffer_put(&tx_buffer, '\r');
    }

    ring_buffer_put(&tx_buffer, c); // Put char in ring buffer for tx

    // If tx not ongoing then start it
    if (empty){
        uart_tx_start();
    }

    // Enable Interrupt to create a mutex
    uart_enable_tx_interrupt();

}
```

UART Transmission is done via ring buffer. Code for this is in ring_buffer.c

LED is toggled using UART Interrupt Handler
```c
void UART0Handler(void){

    if ((UART0_RIS_R & (1<<5)) != 0){
        if (ring_buffer_empty(&tx_buffer)){
            // Assert Error Condition : Interrput called wihtout any transmission
        }

        // Remove data on top of ring buffer
        ring_buffer_get(&tx_buffer);

        // Clear Interrupt
        uart_clear_tx_interrupt();

        // Start next transmission if not empty
        if (!ring_buffer_empty(&tx_buffer)){
            uart_tx_start();
        }
    }

    if ((UART0_RIS_R & (1<<4)) != 0){
        uart_clear_rx_interrupt(); // Clear  Rx Interrupt
        char c;
        c = uart_rx_get();

        // If char passed is r then turn RED LED ON
        if (c=='r'){
            GPIO_PORTF_DATA_R ^= 0x02;
        }

        // If char passed is b then turn BLUE LED ON
        if (c=='b'){
            GPIO_PORTF_DATA_R ^= 0x04;
        }

        // If char passed is g then turn GREEN LED ON
        if (c=='g'){
            GPIO_PORTF_DATA_R ^= 0x08;
        }
        _putchar(c);
    }
}
```
