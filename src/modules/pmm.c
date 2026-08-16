#include<stdint.h>
#include "modules/pmm.h"
#include "modules/uart.h"

// Linker symbols defined in your .ld file
extern char _page_alloc_start[];
extern char _page_alloc_end[];
static uint8_t *current_addr = (uint8_t *)_page_alloc_start;

uint8_t *pmm_alloc_zeroed_page(void){
  if (current_addr + PAGE_SIZE > (uint8_t *)_page_alloc_end){
    uart_puts("Out of physical memory.\n");
    while(1);
  }
  uint8_t *page = current_addr;
  // Zero out the page (PAGE_SIZE / 8 uint64_t entries)
  for (size_t i = 0; i < PAGE_SIZE; i++) {
      page[i] = 0x0;
  }
  // Increment by 4KB for the next page
  current_addr += PAGE_SIZE;
  return page;
}

uint8_t *pmm_alloc_raw_page(void){
  if (current_addr + PAGE_SIZE > (uint8_t *)_page_alloc_end){
    uart_puts("Out of physical memory.\n");
    while(1);
  }
  uint8_t *page = current_addr;
  // Increment by 4KB for the next page
  current_addr += PAGE_SIZE;
  return page;
}


void pmm_free_page(){
  // Set the mem pointer back to start
  current_addr = (uint8_t *)_page_alloc_start;
}
// TODO: Add a better allocator, instead of a simple bump allocator
