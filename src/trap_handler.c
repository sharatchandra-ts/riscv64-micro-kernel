#include <stdint.h>
#include "modules/uart.h"

void trap_handler(uint64_t scause, uint64_t sepc, uint64_t stval){

  // Top bit (63 in RV64) indicates an Interrupt vs Exception
  if (scause & (1ULL << 63)){
    uart_puts("An interrupt occured\n");


  } else {

    uart_puts("An exception occured\n");
    uint64_t exception_id = scause & ~(1ULL << 63);

    switch(exception_id){
      case 2:
        uart_puts("Illegal instruction\n");
        break;

      default:
        uart_puts("Unknown Exception");
    }
  } 

  while (1);
}

