#include "uart.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <ring_buffer.h>

#define UART_BUFFER_SIZE (16)
static uint8_t buffer[UART_BUFFER_SIZE];
static struct ring_buffer tx_buffer = { .buffer = buffer, .size = sizeof(buffer), .head=0, .tail=0 };

#define SYS_CLK (16000000)
#define UART_BAUDRATE (115200)
#define UART_BARUDRATE_DIVISOR ((float)SYS_CLK/(16*UART_BAUDRATE))
#define UART_BAUDRATE_DIVISOR_INT ((uint16_t)UART_BARUDRATE_DIVISOR)
#define UART_BAUDRATE_INT ((uint16_t)UART_BAUDRATE_DIVISOR_INT)
#define UART_BAUDRATE_DIVISOR_FRACTIONAL (UART_BARUDRATE_DIVISOR - UART_BAUDRATE_DIVISOR_INT)
#define UART_BAUDRATE_FRACTIONAL ((uint16_t)(64*UART_BAUDRATE_DIVISOR_FRACTIONAL + 0.5))


void uart_enable_tx_interrupt(void){
    // Set tx interrupt mask bit
    UART0_IM_R |= (1<<5);
}

void uart_disable_tx_interrupt(void){
    // Clear tx interrupt mask bit
    UART0_IM_R &= ~(1<<5);
}

void uart_clear_tx_interrupt(void){
    // Clear the interrupt
    UART0_ICR_R |= (1<<5); // Writing to ICR register clears corresponding interrupt from RIS and MIS
}

void uart_enable_rx_interrupt(void){
    // Set rx interrupt mask bit
    UART0_IM_R |= (1<<4);
}

void uart_disable_rx_interrupt(void){
    // Clear rx interrupt mask bit
    UART0_IM_R &= ~(1<<4);
}

void uart_clear_rx_interrupt(void){
    // Clear the interrupt
    UART0_ICR_R |= (1<<4); // Writing to ICR register clears corresponding interrupt from RIS and MIS
}

void uart_tx_start(void){
    // Transmit the data at top of ring buffer if not empty
    if (!ring_buffer_empty(&tx_buffer)){
        UART0_DR_R |= ring_buffer_peek(&tx_buffer);
    }
}

char uart_rx_get(void){
    char c;
    c = UART0_DR_R;
    return c;
}

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
            GPIO_PORTF_DATA_R = 0x02;
        }

        // If char passed is b then turn BLUE LED ON
        if (c=='b'){
            GPIO_PORTF_DATA_R = 0x04;
        }

        // If char passed is g then turn GREEN LED ON
        if (c=='g'){
            GPIO_PORTF_DATA_R = 0x08;
        }
        _putchar(c);
    }
}


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



void uart_putchar_polling(char c){
    /* UART Flag Register indicates transmission or receiving
     * [ 7 : Transmitting or not | 6 : Receiving or not | 3 : UART Busy or not ] */
    while((UART0_FR_R & 0x80)==0){}
    UART0_DR_R |= c;

    if (c =='\n'){
        uart_putchar_polling('\r');
    }
}

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




