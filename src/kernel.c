#include "modules/uart.h"
#include "modules/time.h"
#include "modules/pmm.h"
#include "modules/vmm.h"

#define TIMER_INTERVAL 1000000ULL 

extern void trap_entry(void);

extern char _stext[], _etext[];
extern char _srodata[], _erodata[];
extern char _sdata[], _edata[];
extern char _sbss[], _ebss[];

extern char _page_alloc_start[], _page_alloc_end[];
extern char _stack_top[], _stack_bottom[];

// Initialize memory addresses for CSRs
void trap_init(void);
void timer_init(void);
void kernel_map_init(void);

int kmain(void){
  timer_init();
  trap_init();
  kernel_map_init();

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

void kernel_map_init(void){
  uint8_t *root = pmm_alloc_page();
  uint64_t root_ppn = ((uint64_t) root) >> PAGE_SHIFT;
 
  // Executable Code (Read + Execute)
  vmm_map_range(root_ppn, (uint64_t)_stext, (uint64_t)_etext, PTE_V | PTE_R | PTE_X);

  // Read-Only Data (Read Only)
  vmm_map_range(root_ppn, (uint64_t)_srodata, (uint64_t)_erodata, PTE_V | PTE_R);

  // Read/Write Data & BSS (Read + Write)
  vmm_map_range(root_ppn, (uint64_t)_sdata, (uint64_t)_edata, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_sbss, (uint64_t)_ebss, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_page_alloc_start, (uint64_t)_page_alloc_end, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_stack_bottom, (uint64_t)_stack_top, PTE_V | PTE_R | PTE_W);
  // NOT PTE_X (never execute a device register).
  vmm_map_range(root_ppn, (uint64_t)UART_BASE, (uint64_t)UART_END, PTE_V | PTE_R | PTE_W);
}
