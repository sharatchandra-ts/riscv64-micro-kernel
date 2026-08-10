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


  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame)
  }

}

/* This is the stack pointer location */
_stack_top = ORIGIN(RAM) + LENGTH(RAM);
_data_load_start = _sdata;
