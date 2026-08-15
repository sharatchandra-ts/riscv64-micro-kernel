#include <stdint.h>
#include "modules/uart.h"
#include "modules/time.h"

#define SCAUSE_INTERRUPT_BIT (1ULL << 63)
#define INTERRUPT_S_TIMER 5LL
#define EXCEPTION_ILLEGAL_INST 2LL
// Define how many timer ticks to wait for the next interrupt
#define TIMER_INTERVAL  100000ULL 

uint64_t trap_handler(uint64_t scause, uint64_t sepc, uint64_t stval){
  uint16_t raw_insn = *(uint16_t *)sepc;
  
  // Top bit (63 in RV64) indicates an Interrupt vs Exception
  if (scause & SCAUSE_INTERRUPT_BIT){
    uint64_t interrupt_id = scause & ~SCAUSE_INTERRUPT_BIT;
    // uart_putf("An interrupt occured with ID: %d\n", interrupt_id);

    switch (interrupt_id) {
      case INTERRUPT_S_TIMER:
        tick_handler();
        // Set the next interrupt intervel 
        sbi_set_timer(get_time() + TIMER_INTERVAL);
        break;
      default:
        uart_putf("An unknown interrupt occurred with ID: %d\n", interrupt_id);
        break;
    }


    // For interrupts, return original sepc to resume where interrupted
    return sepc;

  } else {
    uint64_t exception_id = scause & ~SCAUSE_INTERRUPT_BIT;
    uart_putf("An exception occured with ID: %d, at: %lx, instruction: %lx\n", exception_id, stval, sepc);

    switch(exception_id){
      case EXCEPTION_ILLEGAL_INST:
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

