#include <stdint.h>
#include "modules/uart.h"

uint64_t trap_handler(uint64_t scause, uint64_t sepc, uint64_t stval){
  uint16_t raw_insn = *(uint16_t *)sepc;
  
  // Top bit (63 in RV64) indicates an Interrupt vs Exception
  if (scause & (1ULL << 63)){
    uart_puts("An interrupt occured\n");
    // For interrupts, return original sepc to resume where interrupted
    return sepc;

  } else {

    uint64_t exception_id = scause & ~(1ULL << 63);
    uart_putf("An exception occured with ID: %d\n", exception_id);

    switch(exception_id){
      case 2:
        uart_putf("Exception: Illegal instruction: %d. ", stval);
        uart_puts("Skipping instruction.\n");
        // Normal 4 byte instructions
        if((raw_insn & 0x3) == 0x3) 
          sepc += 4;
        // Special 2 Byte instructions 
        else 
          sepc += 2;

        break;

      default:
        uart_puts("Unknown Exception\n");
        while (1);
    }
  } 
  return sepc;
}

