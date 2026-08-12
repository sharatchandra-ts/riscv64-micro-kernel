#include "modules/uart.h"
#include "modules/time.h"

#define TIMER_INTERVAL 1000000ULL 

extern void trap_entry(void);
extern char _stack_top[];

// Initialize memory addresses for CSRs
void trap_init(void);
void timer_init(void);

int kmain(void){
  timer_init();
  trap_init();

  asm volatile("unimp");

  uart_puts("Hello World! \n");

  while(1);
}


void trap_init(void){
  // Initializes stack space for interrupt operations
  asm volatile("csrw sscratch, %0" :: "r"(_stack_top));
  // Initializes trap entry address
  asm volatile("csrw stvec, %0" :: "r"(trap_entry));
}

void timer_init(void) {
  // Enable Supervisor Timer Interrupts in 'sie' CSR (bit 5 is STIE)
  asm volatile("csrs sie, %0" :: "r"(1ULL << 5));
  // Set the VERY FIRST timer alarm
  sbi_set_timer(get_time() + TIMER_INTERVAL);
  // Globally enable S-mode interrupts in 'sstatus' CSR (bit 1 is SIE)
  asm volatile("csrs sstatus, %0" :: "r"(1ULL << 1));
}
