#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

// This is a bump memory allocator

// Initialize the allocator using linker symbols
void pmm_init(void);

// Allocate a single zeroed 4KB physical page (returns physical address)
uint8_t *pmm_alloc_page(void);

// Free a previously allocated 4KB physical page
void pmm_free_page(void);

#endif // PMM_H
