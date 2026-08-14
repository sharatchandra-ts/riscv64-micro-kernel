#include<stdint.h>
#include "modules/uart.h"
#include "modules/pmm.h"

// PTE Bit Flags
#define PTE_V (1 << 0) // Valid
#define PTE_R (1 << 1) // Read
#define PTE_W (1 << 2) // Write
#define PTE_X (1 << 3) // Execute
#define PTE_U (1 << 4) // User
#define PTE_G (1 << 5) // Global
#define PTE_A (1 << 6) // Accessed
#define PTE_D (1 << 7) // Dirty

#define PAGE_SHIFT 12

// Extracts the Virtual Page Number (VPN) for level i (i = 2, 1, 0)
static inline uint64_t get_vpn(uint64_t va, int level) {
    return (va >> (PAGE_SHIFT + level * 9)) & 0x1FF;
}

// Extracts the Physical Page Number (PPN) from a PTE
static inline uint64_t pte_to_ppn(uint64_t pte) {
    return (pte >> 10) & 0xFFFFFFFFFFF; // Bits 53:10
}

// Create a PTE from an existing PPN
static inline uint64_t ppn_to_pte(uint64_t ppn) {
    return (ppn << 10); // Bits 53:10
}



uint64_t *page_walk(uint64_t root_ppn, uint64_t va) {
  uint64_t current_ppn = root_ppn;
  
  // Sv39 has 3 levels: Level 2 (1GB), Level 1 (2MB), Level 0 (4KB)
  for (int level = 2; level > 0; level--) {
    uint64_t vpn = get_vpn(va, level);
    
    // Calculate Physical Address of the PTE
    uint64_t pte_paddr = (current_ppn << PAGE_SHIFT) + (vpn * 8);
    uint64_t *pte_ptr = (uint64_t *) pte_paddr;

    // Check if PTE is Valid
    if (!(*pte_ptr & PTE_V)) {
      uint64_t new_page_entry = pmm_alloc_page();
      *pte_ptr = ppn_to_pte(new_page_entry) | PTE_V;
    }
    // Advance current_ppn to the child page table for the next iteration
    current_ppn = pte_to_ppn(*pte_ptr);
  }

  uint64_t vpn0 = get_vpn(va, 0);
  uint64_t *pte_ptr0 = (uint64_t *) pte0_paddr;

  return pte_ptr0;
}



uint64_t page_translate(uint64_t root_ppn, uint64_t va) {
  uint64_t current_ppn = root_ppn;
  
  // Sv39 has 3 levels: Level 2 (1GB), Level 1 (2MB), Level 0 (4KB)
  for (int level = 2; level >= 0; level--) {
    uint64_t vpn = get_vpn(va, level);
    
    // Calculate Physical Address of the PTE
    uint64_t pte_paddr = (current_ppn << PAGE_SHIFT) + (vpn * 8);
    uint64_t pte = *(uint64_t *) pte_paddr;

    // Check if PTE is Valid
    if (!(pte & PTE_V)) {
      // Page Fault: Invalid entry
      uart_puts("Page Fault: Invalid entry\n");
      while(1);
    }

    // Check for Reserved Flag combinations (W=1 and R=0 is invalid)
    if (!(pte & PTE_R) && (pte & PTE_W)) {
      // Page Fault: Reserved configuration
      uart_puts("Page Fault: Reserved configuration\n");
      while(1);
    }

    // Check if this is a Leaf Node (R=1, W=1, or X=1)
    if ((pte & (PTE_R | PTE_W | PTE_X)) != 0) {
      uint64_t pte_ppn = pte_to_ppn(pte);
      if(level != 0){
        // Page Fault:
        uart_puts("Page Fault: Superpages not supported yet.\n");
        while(1);
      }

      uint64_t offset = va & 0xFFF;
      return (pte_ppn << PAGE_SHIFT) | offset;
    }


    if (level == 0) {
      uart_puts("Page Fault: Missing leaf at level 0\n");
      while(1);
    }

    // Non-leaf node: Update root PPN and descend to next level
    current_ppn = pte_to_ppn(pte);
  }
  return 0;

}

