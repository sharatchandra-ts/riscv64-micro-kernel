/* Linker script */

/*
 * Defined in config file
 * RAM starting address: 0x80200000 
 * RAM length          : 128 MB
 */

ENTRY(_start)

MEMORY {
  RAM : ORIGIN = RAM_START_ADDR, LENGTH = RAM_SIZE 
}

SECTIONS {
  .text : {
    . = ALIGN(4096);
    _stext = .;
    KEEP(*(.text.boot))
    KEEP(*(.text.startup))
    KEEP(*(.init))
    *(.text .text*)
    . = ALIGN(4096);
    _etext = .;
  } > RAM


  .rodata : {
    . = ALIGN(4096);
    _srodata = .;
    *(.rodata .rodata*)
    . = ALIGN(4096);
    _erodata = .;
  } > RAM


  .data : {
    . = ALIGN(4096);
    _sdata = .; 
    *(.data .data*)
    *(.sdata .sdata.*)
    . = ALIGN(4096);
    _edata = .;
  } > RAM 


  .bss : {
    . = ALIGN(4096);
    _sbss = .;
    *(.sbss .sbss.*)
    *(.bss .bss*)
    *(COMMON)
    . = ALIGN(4096);
    _ebss = .;
  } > RAM

  
  . = ALIGN(4096);
  _page_alloc_start = .;


  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame)
  }

}

/* Fixed stack space at top of RAM */
__stack_size = 0x10000; /* 64 KB  Kernel Stack */
/* This is the stack pointer location */
_stack_top = ORIGIN(RAM) + LENGTH(RAM);
_stack_bottom = _stack_top - __stack_size;

/* End of the page allocator memory pool */
_page_alloc_end = _stack_bottom;
