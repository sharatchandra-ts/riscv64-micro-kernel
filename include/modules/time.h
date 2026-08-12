#ifndef SBI_TIMER_H
#define SBI_TIMER_H

#include <stdint.h>

// SBI Timer Extension Constants (RISC-V SBI Specification v0.2+)
#define SBI_EXT_TIME         0x54494D45ULL
#define SBI_FID_SET_TIMER    0ULL
// System Configuration
#define TIMER_FREQ       10000000ULL // 10 MHz on QEMU virt
#define TICK_RATE_HZ     100ULL      // Want 100 ticks per second (1 tick = 10 ms)
#define TICKS_PER_INT    (TIMER_FREQ / TICK_RATE_HZ) // 100,000 hardware ticks per interrupt

// Global System Time Accounting
static volatile uint64_t g_ticks = 0;


/**
 * @brief Reads the current RISC-V hardware time CSR.
 * @return Current 64-bit cycle/time counter value.
 */
static inline uint64_t get_time(void) {
    uint64_t n;
    asm volatile("csrr %0, time" : "=r"(n));
    return n;
}

/**
 * @brief Issues an SBI call to set the next timer interrupt target.
 * @param stime_value Absolute timestamp (in cycles/ticks) when the interrupt should trigger.
 */
static inline void sbi_set_timer(uint64_t stime_value) {
    register uint64_t a0 asm ("a0") = stime_value;
    register uint64_t a1 asm ("a1") = 0;
    register uint64_t a6 asm ("a6") = SBI_FID_SET_TIMER;
    register uint64_t a7 asm ("a7") = SBI_EXT_TIME;

    asm volatile(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a6), "r"(a7)
        : "memory"
    );
}

void tick_handler(void);

uint64_t get_uptime_ms(void);

#endif // SBI_TIMER_H
