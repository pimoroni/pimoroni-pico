#include "py/misc.h"

// The cluster blocks come from the GC heap, PSRAM included, which the DMA reads as it does
// SRAM. Exhaustion surfaces as a clean init failure with gc.collect() as the remedy.

void *pwm_cluster_allocate(size_t size) {
    return m_malloc_maybe(size);
}

void pwm_cluster_deallocate(void *mem) {
    m_free(mem);
}
