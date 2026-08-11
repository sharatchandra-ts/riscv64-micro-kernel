#include "modules/uart.h"

extern void trap_entry(void);
extern char* _stack_top;

// Initialize memory addresses for CSRs
void trap_init(void);


int kmain(void){
  trap_init();

  uart_puts("Hello World! \n");

  asm volatile("unimp");

  while(1);
}


void trap_init(void){
  // Initializes stack space for interrupt operations
  asm volatile("csrw sscratch, %0" :: "r"(_stack_top));
  // Initializes trap entry address
  asm volatile("csrw stvec, %0" :: "r"(trap_entry));
}
