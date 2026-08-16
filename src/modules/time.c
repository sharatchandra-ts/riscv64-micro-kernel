#include <stdint.h>
#include "modules/time.h"
#include "modules/uart.h"


void tick_handler(void){
  // Global uptime
  g_ticks++;

  // Every 100 ticks is a second
  // if(g_ticks % 100 == 0)
    // uart_putf("Uptime: %d seconds (%d ticks)\n", (g_ticks / 100), g_ticks);
  
}

// Helper to query system uptime in milliseconds
uint64_t get_uptime_ms(void) {
    return (g_ticks * 1000ULL) / TICK_RATE_HZ;
}
