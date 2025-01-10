## Question

*UART Transmit :* Program your micro-controller to transmit the 8-bit value "0xF0" if SW1 is pressed and "0xAA" if SW2 is pressed over UART with baud rate 9600 and odd parity. Read the relevant sections of the datasheet and board manual.

*UART Receive :* Write code for incoming data on the UART with the same baud and parity config. If "0xAA" is received, turn LED should light up GREEN. If "0xF0" is received, the LED should be BLUE and if any error is detected LED should be RED. 

## Solution

*UART Transmit :*

Initialize GPIO Port F with Interrupt functionality on switches as in before examples.
Initialize GPIO UART for transmitting

```c
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
```

Whenever a switch is pressed, GPIO Interrupt Handler starts. 
In this handler add UART Transmisson code as follows
```c
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
```

*UART Receive :*

Initialize GPIO Port F for LED Control
```c
void LED_Init(void) {
    SYSCTL_RCGCGPIO_R |= (1 << 5);
    while ((SYSCTL_PRGPIO_R & (1 << 5)) == 0);
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
}
```

Initialize UART to receive incoming transmission from computer
```c
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
```

Poll on UART receive to check if correct data for LED control is received, if so pass control to LED control code.
```c
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
```

To transmit data via Code Composer Studio, click on terminal and select serial terminal with ASCII. In this apply same UART settings as used for this code.

