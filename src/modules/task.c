#include "modules/task.h"
#include "modules/uart.h"
#include "modules/pmm.h"

extern void switch_to_task(context_t *old_ctx, context_t *new_ctx);

static Task tasks[MAX_TASKS];
// -1 = "no task running yet, we're in kmain"
static int current_task = -1;                               

void task_init(void){
  for(int i=0; i < MAX_TASKS; i++){
    tasks[i].state = TASK_STATE_UNUSED;
  } 
}

int task_create(void (*entry_point)(void)){
  for (int i=0; i < MAX_TASKS; i++){
    if (tasks[i].state == TASK_STATE_UNUSED){
      // TODO: change from pmm to vmm with virtual addresses
      uint8_t *task_stack = pmm_alloc_zeroed_page();

      tasks[i].pid = i;
      tasks[i].state = TASK_STATE_READY;
      tasks[i].context.ra = (uint64_t) entry_point;
      tasks[i].context.sp = (uint64_t) task_stack + TASK_STACK_SIZE;
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
