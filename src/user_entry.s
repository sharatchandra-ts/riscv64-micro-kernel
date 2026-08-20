.global enter_user_mode

# void enter_user_mode(uint64_t entry_point, uint64_t user_sp);
#   a0 = entry_point  (virtual address of code, e.g., TASK_TEXT_VA)
#   a1 = user_sp      (virtual address of stack top, e.g., TASK_STACK_VA + size)
#   a2 = kernel_sp    (virtual address of kernel top)

enter_user_mode:
  # Set target U-mode entry address into sepc
  csrw sepc, a0

  # Set Previous Privilege Mode (SPP) to U-mode (0)
  li t0, (1 << 8)
  csrc sstatus, t0

  # Preserve interrupt state upon transition
  li t0, (1 << 5)
  csrs sstatus, t0

  # Pass the user stack pointer to U-mode's sp register
  mv sp, a1

  csrw sscratch, a2

  # Clean registers to prevent leaking kernel memory addresses to user space
  li a0, 0
  li a1, 0
  li t0, 0

  # Drop privilege to U-mode and jump to sepc (entry_point)
  sret


.global user_task_entry_trampoline
user_task_entry_trampoline:
  li a0, 0x00010000       # TASK_TEXT_VA
  li a1, 0x40002000       # TASK_STACK_VA top
  li a2, 0x40001000       # TASK_KERNEL_STACK_VA top
  # TODO: This is a temperorary function, with fixed values, fix later 
  j enter_user_mode
