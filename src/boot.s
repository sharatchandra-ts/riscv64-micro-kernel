# boot.s 

.section .text.boot
.global _start


_start:
  # Initilise stack pointer
  la  sp, _stack_top

  # Start and end location of .bss
  la  t0, _sbss
  la  t1, _ebss

clear_bss:
  bgeu t0, t1, end_clear_bss

  # Initilises all values to 0
  sd  zero, 0(t0)

  # Advance pointer by 8 bytes
  addi  t0, t0, 8
  j clear_bss

end_clear_bss:

  # Jump to C entry point 
  call kmain

hang:
  wfi 
  j hang
