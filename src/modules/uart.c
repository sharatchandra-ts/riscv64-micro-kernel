#include <stdarg.h>
#include "modules/uart.h"

// Hardware Register Definitions
#define UART_BASE     0x10000000UL
#define UART_THR      ((volatile uint8_t *)(UART_BASE + 0x00)) // Transmit Holding Register
#define UART_LSR      ((volatile uint8_t *)(UART_BASE + 0x05)) // Line Status Register

// Status Register Bits
#define UART_LSR_THRE 0x20 // Bit 5: Transmitter Holding Register Empty

void uart_putc(char c) {
    // Wait until the Transmit Holding Register is empty (Bit 5 = 1)
    while (!(*UART_LSR & UART_LSR_THRE));

    // Write character to buffer
    *UART_THR = (uint8_t)c;
}

void uart_puts(const char* s) {
    while (*s != '\0') {
      uart_putc(*s);
      s++;
    }
}

void uart_putf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
      if (*fmt == '%' && *(fmt + 1) != '\0') {
        fmt++; // Move past '%'
        switch (*fmt) {
          case 'c': {
            // Fetch as int due to default argument promotion
            char c = (char)va_arg(args, int);
            uart_putc(c);
            break;
          }
          case 'd': {
            int num = va_arg(args, int);
            uart_puti(num);
            break;
          }
          case 's': {
            char *s = va_arg(args, char *);
            if (s == (char*)0) {
                s = "(null)";
            }
            // Print raw string directly
            uart_puts(s); 
            break;
          }
          case '%': {
            uart_putc('%');
            break;
          }
          default: {
            uart_putc('%');
            uart_putc(*fmt);
            break;
          }
        }
      } else {
          uart_putc(*fmt);
      }
        fmt++;
    }

    va_end(args);
}

void uart_puti(const int i) {
    char str[16];
    str[15] = '\0';
    int j = 0;

    // Use an unsigned int to safely handle negative values without overflow (e.g., INT_MIN)
    unsigned int num;
    // Handle negative numbers
    if (i < 0) {
      uart_putc('-');
      num = (unsigned int)(-i);
    } else {
      num = (unsigned int)i;
    }

    // Convert digits from right to left using a do-while loop
    // (do-while guarantees that passing '0' still prints '0')
    do {
      j++;
      str[15 - j] = '0' + (num % 10);
      num /= 10;
    } while (num > 0 && j < 15);

    uart_puts(&str[15 - j]);
}
