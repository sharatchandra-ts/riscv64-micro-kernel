#include "modules/uart.h"
#include "modules/time.h"
#include "modules/pmm.h"
#include "modules/vmm.h"
#include "modules/task.h"

#define TIMER_INTERVAL 1000000ULL

extern void trap_entry(void);
extern void switch_to_task(context_t *old_ctx, context_t *new_ctx);

extern char _stext[], _etext[];
extern char _srodata[], _erodata[];
extern char _sdata[], _edata[];
extern char _sbss[], _ebss[];

extern char _page_alloc_start[], _page_alloc_end[];
extern char _stack_top[], _stack_bottom[];

static uint64_t root_ppn;

// Initialize memory addresses for CSRs
void trap_init(void);
void timer_init(void);
void kernel_map_init(void);
void satp_init(void);
void taskA(void);
void taskB(void);

static context_t taskA_ctx, taskB_ctx, kernel_ctx;

void taskA(void){
    uart_puts("task: A\n");
    switch_to_task(&taskA_ctx, &taskB_ctx);
}

void taskB(void){
    uart_puts("task: B\n");
    switch_to_task(&taskB_ctx, &kernel_ctx);
}

int kmain(void){
  timer_init();
  trap_init();
  kernel_map_init();
  uart_puts("Hello World! \n");
  satp_init();
  uart_puts("Paging enabled successfully!\n");

  uint8_t *taskA_stack = pmm_alloc_zeroed_page();
  uint8_t *taskB_stack = pmm_alloc_zeroed_page();

  taskA_ctx.ra = (uint64_t) taskA;
  taskA_ctx.sp = (uint64_t) taskA_stack + 4096;
  taskB_ctx.ra = (uint64_t) taskB;
  taskB_ctx.sp = (uint64_t) taskB_stack + 4096;

  switch_to_task(&kernel_ctx, &taskA_ctx);

  uart_puts("Returned back to kernel!!\n");

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
  uint8_t *root = pmm_alloc_zeroed_page();
  root_ppn = ((uint64_t) root) >> PAGE_SHIFT;

  // Executable Code (Read + Execute)
  vmm_map_range(root_ppn, (uint64_t)_stext, (uint64_t)_etext, PTE_V | PTE_R | PTE_X);

  // Read-Only Data (Read Only)
  vmm_map_range(root_ppn, (uint64_t)_srodata, (uint64_t)_erodata, PTE_V | PTE_R);

  // Read/Write Data & BSS (Read + Write)
  vmm_map_range(root_ppn, (uint64_t)_sdata, (uint64_t)_edata, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_sbss, (uint64_t)_ebss, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_stack_bottom, (uint64_t)_stack_top, PTE_V | PTE_R | PTE_W);
  // NOT PTE_X (never execute a device register).
  vmm_map_range(root_ppn, (uint64_t)UART_BASE, (uint64_t)UART_END, PTE_V | PTE_R | PTE_W);
  vmm_map_range(root_ppn, (uint64_t)_page_alloc_start, (uint64_t)_page_alloc_end, PTE_V | PTE_R | PTE_W);
}


void satp_init(void){
  uint64_t satp_entry = (8ULL << 60) | root_ppn;
  // Write to satp (Supervisor Address Translation and Protection)
  asm volatile("csrw satp, %0" :: "r"(satp_entry));
  // Flush the TLB immediately after enabling paging
  asm volatile("sfence.vma" ::: "memory");
}
