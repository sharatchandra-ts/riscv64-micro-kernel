.global enter_user_mode


enter_user_mode:
  # a0 = entry address, a1 = stack pointer
  mv   sp, a1
  # Set the target return address (passed in a0) into sepc
  csrw sepc, a0
  # 0x100 is (1 << 8), which targets the SPP bit
  li t0, 0x100            
  csrc sstatus, t0

  # Execute the return to transition privilege modes
  sret
