#ifndef VMM_H
#define VMM_H

#include <stdint.h>

/**
 * Virtual Memory Manager (VMM) for RISC-V Sv39 Paging.
 * Provides page table traversal, page mapping, and address translation.
 */

// PTE Permission Flags
#define PTE_V (1 << 0) // Valid entry
#define PTE_R (1 << 1) // Read access
#define PTE_W (1 << 2) // Write access
#define PTE_X (1 << 3) // Execute access
#define PTE_U (1 << 4) // User-mode access
#define PTE_G (1 << 5) // Global mapping
#define PTE_A (1 << 6) // Accessed flag
#define PTE_D (1 << 7) // Dirty flag

#define PAGE_SHIFT 12

/**
 * @brief Walks the 3-level page table tree for a Virtual Address.
 * Allocates missing intermediate page tables as needed via pmm_alloc_zeroed_page().
 *
 * @param root_ppn Physical Page Number of the root (level 2) page table
 * @param va Virtual Address to look up
 * @return uint64_t* Pointer to the Level-0 PTE slot
 */
uint64_t *vmm_walk(uint64_t root_ppn, uint64_t va);

/**
 * @brief Maps a virtual page to a physical address with given permissions.
 * Traverses tables using vmm_walk and writes the leaf PTE entry at Level-0.
 *
 * @param root_ppn Physical Page Number of the root (level 2) page table
 * @param va Target Virtual Address to map
 * @param pa Target Physical Address to map to
 * @param flags Permission flags to set (e.g., PTE_R | PTE_W)
 */
void vmm_map_page(uint64_t root_ppn, uint64_t va, uint64_t pa, uint64_t flags);

/**
 * @brief Translates a Virtual Address to its mapped Physical Address.
 * Software MMU emulation used for kernel debugging and fault handling.
 *
 * @param root_ppn Physical Page Number of the root (level 2) page table
 * @param va Virtual Address to translate
 * @return uint64_t Corresponding Physical Address including page offset
 */
uint64_t vmm_virt_to_phys(uint64_t root_ppn, uint64_t va);


void vmm_map_range(uint64_t root_ppn, uint64_t start, uint64_t end, uint64_t flags);

// Allocate a single zeroed and mapped 4KB virtual page (returns address)
uint64_t vmm_alloc_page(uint64_t root_ppn, uint64_t va, uint64_t perm_flags);

#endif // VMM_H
