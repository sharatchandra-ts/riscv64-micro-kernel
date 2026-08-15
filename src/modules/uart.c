#include <stdarg.h>
#include <stdint.h>
#include "modules/uart.h"

// Hardware Register Definitions
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

// Helper to convert dynamic 64-bit unsigned int to hexadecimal
void uart_puthex64(uint64_t val, int prefix_0x) {
  if (prefix_0x) {
    uart_puts("0x");
  }

  if (val == 0) {
    uart_putc('0');
    return;
  }

  char buf[17]; // 16 hex digits + null terminator
  buf[16] = '\0';
  int idx = 16;

  while (val > 0) {
    uint8_t nibble = val & 0xF;
    idx--;
    buf[idx] = (nibble < 10) ? ('0' + nibble) : ('a' + (nibble - 10));
    val >>= 4;
  }

  uart_puts(&buf[idx]);
}

void uart_putf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == '%' && *(fmt + 1) != '\0') {
            fmt++; // Move past '%'

            // Check for width specifiers like 'l' (long) or 'll' (long long)
            int is_64bit = 0;
            if (*fmt == 'l') {
                fmt++;
                if (*fmt == 'l') {
                    fmt++;
                }
                is_64bit = 1;
            }

            switch (*fmt) {
                case 'c': {
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
                    uart_puts(s);
                    break;
                }
                case 'x':
                case 'X': {
                    uint64_t val = is_64bit ? va_arg(args, uint64_t) : va_arg(args, unsigned int);
                    uart_puthex64(val, 0);
                    break;
                }
                case 'p': {
                    uint64_t ptr = (uint64_t)va_arg(args, void *);
                    uart_puthex64(ptr, 1);
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
