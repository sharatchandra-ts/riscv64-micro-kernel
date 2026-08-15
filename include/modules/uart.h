#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BASE     0x10000000UL
#define UART_END      0x10000FFFUL

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

/**
 * @brief Formats and transmits a string over UART using format specifiers.
 * Supports %c (character), %s (string), %d (integer), and %% (literal %).
 * 
 * @param fmt Format string containing literal text and specifiers
 * @param ... Variable arguments corresponding to the format specifiers
 */
void uart_putf(const char* fmt, ...);

/**
 * @brief Transmits a signed integer as an ASCII string over UART.
 * Converts the integer to base-10 characters and sends them sequentially.
 * 
 * @param i Integer value to print
 */
void uart_puti(const int i);

#endif // UART_H
