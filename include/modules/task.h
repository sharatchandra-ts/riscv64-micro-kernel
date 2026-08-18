#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 4
#define TASK_STACK_VA 0x40000000UL
#define MAX_TASK_PAGES 250

typedef enum {
  TASK_STATE_UNUSED = 0,
  TASK_STATE_READY,
  TASK_STATE_RUNNING,
  TASK_STATE_BLOCKED
} task_state_t;

// Saved Execution Context (Full Trap Frame style)
typedef struct {
  uint64_t ra;  // x1
  uint64_t sp;  // x2
  uint64_t gp;  // x3
  uint64_t tp;  // x4
  uint64_t s0;  // x8
  uint64_t s1;  // x9  
  uint64_t s2;  // x18
  uint64_t s3;  // x19
  uint64_t s4;  // x20
  uint64_t s5;  // x21
  uint64_t s6;  // x22
  uint64_t s7;  // x23
  uint64_t s8;  // x24
  uint64_t s9;  // x25
  uint64_t s10; // x26
  uint64_t s11; // x27
  uint64_t satp;// Holds the root table addr
} context_t;


// Control Block for a Task (TCB)
typedef struct {
  uint32_t     pid;
  task_state_t state;
  context_t    context;
} Task;

extern context_t kernel_ctx;

// Public Task Management API
void tasks_init(void);

int task_create(void (*entry_point)(void), int stack_pages, uint64_t flags);

void task_exit(void);

void yield(void);

#endif // TASKS_H
