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
    . = ALIGN(8);
    
    KEEP(*(.text.boot))
    KEEP(*(.text.startup))
    KEEP(*(.init))
    *(.text .text*)
    *(.rodata .rodata*)
    _etext = .;

    . = ALIGN(8);
  } > RAM


  .data : {
    . = ALIGN(8);

    _sdata = .; 
    *(.data .data*)
    *(.sdata .sdata.*)
    _edata = .;

    . = ALIGN(8);
  } > RAM 


  .bss : {
    . = ALIGN(8);
   
    _sbss = .;
    *(.sbss .sbss.*)
    *(.bss .bss*)
    *(COMMON)
    _ebss = .;

    . = ALIGN(8);
  } > RAM

  
  /* Aligning 4KB for paging (each page is 4KB) */
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
