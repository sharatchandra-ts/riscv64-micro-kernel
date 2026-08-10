#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
 * @brief Transmits a single character over UART.
 * Blocks until the Transmit Holding Register is empty.
 * 
 * @param c Character to print
 */
void uart_putc(char c);

/**
 * @brief Transmits a null-terminated string over UART.
 * 
 * @param s Pointer to the null-terminated string
 */
void uart_puts(const char* s);

#endif // UART_H
