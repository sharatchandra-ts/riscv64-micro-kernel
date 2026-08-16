#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

// This is a bump memory allocator
// NOTE: pmm_alloc_page() only returns a physical address.
// Post-satp, this page is UNMAPPED until you explicitly call
// vmm_map_range()/map_page() on it. Never assume a page is
// usable just because allocation succeeded — always map before use.

// Allocate a single zeroed 4KB physical page (returns physical address)
uint8_t *pmm_alloc_zeroed_page(void);

// Allocate a single non-zeroed 4KB physical page (returns physical address)
uint8_t *pmm_alloc_raw_page(void);

// Free a previously allocated 4KB physical page
void pmm_free_page(void);

#endif // PMM_H
