.global switch_to_task

# void switch_to_task(context_t *old_ctx, context_t *new_ctx);
#   a0 = pointer to current task's context struct (old_ctx)
#   a1 = pointer to next task's context struct (new_ctx)

switch_to_task:
  # Save Current Context to old_ctx (a0)
  sd ra,   0(a0) # x1 - Return Address
  sd sp,   8(a0) # x2 - Stack Pointer
  sd gp,  16(a0) # x3 - Global Pointer
  sd tp,  24(a0) # x4 - Thread Pointer
  sd s0,  32(a0) # x8 - Frame Pointer / s0
  sd s1,  40(a0) # x9
  sd s2,  48(a0) # x18
  sd s3,  56(a0) # x19
  sd s4,  64(a0) # x20
  sd s5,  72(a0) # x21
  sd s6,  80(a0) # x22
  sd s7,  88(a0) # x23
  sd s8,  96(a0) # x24
  sd s9, 104(a0) # x25
  sd s10,112(a0) # x26
  sd s11,120(a0) # x27

  # Restore Target Context from new_ctx (a1)
  ld ra,   0(a1)
  ld sp,   8(a1)
  ld gp,  16(a1)
  ld tp,  24(a1)
  ld s0,  32(a1)
  ld s1,  40(a1)
  ld s2,  48(a1)
  ld s3,  56(a1)
  ld s4,  64(a1)
  ld s5,  72(a1)
  ld s6,  80(a1)
  ld s7,  88(a1)
  ld s8,  96(a1)
  ld s9, 104(a1)
  ld s10,112(a1)
  ld s11,120(a1)
 
  ld   t0, 128(a1)
  csrw satp, t0 
  sfence.vma

  li t0, 2 # 1<<1, i.e. the SIE bit
  csrs sstatus, t0

  # Resume Execution
  ret            # Jumps to restored ra (0(a1))
