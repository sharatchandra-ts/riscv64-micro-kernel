# trap.s
.section .text


.global trap_entry

.align 2
trap_entry:

  # Swap user sp with kernel sp stored in sscratch, use this when multiple stacks exist
  csrrw sp, sscratch, sp

  # Allocate stack space for general registers (31 registers * 8 bytes = 248 bytes)
  addi sp, sp, -256


  # Save all the registers onto the stack
  sd x1,  0(sp)
  # sd x2,  8(sp)
  sd x3,  16(sp)
  sd x4,  24(sp)
  sd x5,  32(sp)
  sd x6,  40(sp)
  sd x7,  48(sp)
  sd x8,  56(sp)
  sd x9,  64(sp)
  sd x10, 72(sp)
  sd x11, 80(sp)
  sd x12, 88(sp)
  sd x13, 96(sp)
  sd x14, 104(sp)
  sd x15, 112(sp)
  sd x16, 120(sp)
  sd x17, 128(sp)
  sd x18, 136(sp)
  sd x19, 144(sp)
  sd x20, 152(sp)
  sd x21, 160(sp)
  sd x22, 168(sp)
  sd x23, 176(sp)
  sd x24, 184(sp)
  sd x25, 192(sp)
  sd x26, 200(sp)
  sd x27, 208(sp)
  sd x28, 216(sp)
  sd x29, 224(sp)
  sd x30, 232(sp)
  sd x31, 240(sp)


  # Retrieve the original user sp from sscratch and save it
  csrr  t0, sscratch
  sd    t0, 8(sp)

  # Pass scause, sepc, and stval as arguments to the C function
  csrr  a0, scause
  csrr  a1, sepc
  csrr  a2, stval


  # Call the C high-level trap handler
  # Arguments: a0 = scause, a1 = sepc, a2 = stval
  call trap_handler


  # Update sepc in case the C handler modified the return address
  csrw  sepc, a0


  # Restore registers (Skip x2/sp if relying on addi sp, sp, 248 later)
  ld x1,   0(sp)
  # ld x2, 8(sp)  Skip overwriting sp here so stack index arithmetic works
  ld x3,   16(sp)
  ld x4,   24(sp)
  ld x5,   32(sp)
  ld x6,   40(sp)
  ld x7,   48(sp)
  ld x8,   56(sp)
  ld x9,   64(sp)
  ld x10,  72(sp)
  ld x11,  80(sp)
  ld x12,  88(sp)
  ld x13,  96(sp)
  ld x14,  104(sp)
  ld x15,  112(sp)
  ld x16,  120(sp)
  ld x17,  128(sp)
  ld x18,  136(sp)
  ld x19,  144(sp)
  ld x20,  152(sp)
  ld x21,  160(sp)
  ld x22,  168(sp)
  ld x23,  176(sp)
  ld x24,  184(sp)
  ld x25,  192(sp)
  ld x26,  200(sp)
  ld x27,  208(sp)
  ld x28,  216(sp)
  ld x29,  224(sp)
  ld x30,  232(sp)
  ld x31,  240(sp)

  # Restore user sp back to sscratch
  ld    t0, 8(sp)
  csrw  sscratch, t0


  # Deallocate stack frame
  addi sp, sp, 256

  # Swap back to the user stack pointer
  csrrw sp, sscratch, sp

  # Return to user space
  sret
