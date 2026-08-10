#include "modules/uart.h"

int kmain(void){

  uart_putc('0');
  uart_putc('\n');
  uart_puts("Hello World! \n");

  while(1);
}
