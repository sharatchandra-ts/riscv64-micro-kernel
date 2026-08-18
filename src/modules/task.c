#include "modules/task.h"
#include "modules/uart.h"
#include "modules/pmm.h"
#include "modules/vmm.h"


extern void switch_to_task(context_t *old_ctx, context_t *new_ctx);

static Task tasks[MAX_TASKS];
// -1 = "no task running yet, we're in kmain"
static int current_task = -1;                               

void task_init(void){
  for(int i=0; i < MAX_TASKS; i++){
    tasks[i].state = TASK_STATE_UNUSED;
  } 
}

int task_create(void (*entry_point)(void), int stack_pages, uint64_t flags){
  if (stack_pages > MAX_TASK_PAGES) {
      uart_puts("Requested stack pages exceed limit. Clamping to MAX_TASK_PAGES.\n");
      stack_pages = MAX_TASK_PAGES;
  }

  for (int i = 0; i < MAX_TASKS; i++) {
    if (tasks[i].state == TASK_STATE_UNUSED) {
      uint64_t root_ppm_local = vmm_create_root_table();

      for (int p = 0; p < stack_pages; p++) {
          uint64_t va = TASK_STACK_VA + ((uint64_t)p * PAGE_SIZE);
          vmm_alloc_page(root_ppm_local, va, PTE_R | PTE_W | flags);
      }

      uint64_t stack_top = TASK_STACK_VA + ((uint64_t)stack_pages * PAGE_SIZE);

      tasks[i].pid = i;
      tasks[i].state = TASK_STATE_READY;
      tasks[i].context.ra = (uint64_t) entry_point;
      tasks[i].context.sp = stack_top;
      tasks[i].context.satp = (8ULL << 60) | root_ppm_local;
      return i;
    }
  }
  uart_puts("TASK LIST is full\n");
  return -1;
}

void yield(void){
  int old_task = current_task;

  if (old_task != -1) {
    tasks[old_task].state = TASK_STATE_READY;
  }

  int next_task = -1;
  for (int i = 1; i <= MAX_TASKS; i++) {
    int idx = (old_task + i) % MAX_TASKS;
    if (tasks[idx].state == TASK_STATE_READY) {
      next_task = idx;
      break;
    }
  }

  if (next_task == -1){
    uart_puts("No ready tasks to yield to!\n");
    return;
  }
  tasks[next_task].state = TASK_STATE_RUNNING;
  current_task = next_task;

  // Special case: old_task == -1 means we're yielding FROM kmain,
  // not from a real task slot — need a separate context for that.
  if (old_task == -1) {
    switch_to_task(&kernel_ctx, &tasks[next_task].context);
  } else {
    switch_to_task(&tasks[old_task].context, &tasks[next_task].context);
  }
}

void task_exit(void){
  int old_task = current_task;
  tasks[old_task].state = TASK_STATE_UNUSED;

  int next_task = -1;
  for (int i = 1; i <= MAX_TASKS; i++) {
    int idx = (old_task + i) % MAX_TASKS;
    if (tasks[idx].state == TASK_STATE_READY) {
      next_task = idx;
      break;
    }
  }

  if (next_task == -1) {
    uart_puts("No ready tasks left after exit!\n");
    while(1);  // nothing left to run — halt, don't return
  }

  tasks[next_task].state = TASK_STATE_RUNNING;
  current_task = next_task;

  // Note: passing &tasks[old_task].context as a throwaway —
  // its saved state will never be read again, but switch_to_task
  // needs SOME valid pointer to write into.
  switch_to_task(&tasks[old_task].context, &tasks[next_task].context);
}
