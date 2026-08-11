#include <stdint.h>
#include "modules/uart.h"

void trap_handler(uint64_t scause, uint64_t sepc, uint64_t stval){

  // Top bit (63 in RV64) indicates an Interrupt vs Exception
  if (scause & (1ULL << 63)){
    uart_puts("An interrupt occured\n");


  } else {

    uint64_t exception_id = scause & ~(1ULL << 63);
    uart_puts("An exception occured with ID: ");
    uart_puti(exception_id);
    uart_putc('\n');

    switch(exception_id){
      case 2:
        uart_puts("Exception: Illegal instruction\n");
        break;

      default:
        uart_puts("Unknown Exception");
        break;
    }
  } 

  while (1);
}

