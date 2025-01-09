#ifndef UART_H
#define UART_H

void uart_init(void); // Initialize trace
void uart_putchar_polling(char c); // UART putchar using polling function
void _putchar(char c); // UART putchar using interrupt function


#endif /* UART_H_ */
