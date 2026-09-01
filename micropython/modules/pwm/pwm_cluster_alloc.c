#include "py/misc.h"
#include "py/gc.h"
#include "hardware/regs/addressmap.h"

extern void *__real_malloc(size_t size);
extern void __real_free(void *mem);

// PWMCluster's buffer block is a DMA source, so it must be in memory the DMA reads
// coherently with the CPU's writes. The GC heap is preferred: it is SRAM on most boards,
// and exhaustion there surfaces as a clean init failure with gc.collect() as the remedy.
// On boards whose GC heap is external PSRAM the DMA would read stale data through the XIP
// cache, so those take the C heap instead, which the port always places in SRAM.

static bool in_sram(const void *mem) {
    return (uintptr_t)mem >= SRAM_BASE && (uintptr_t)mem < SRAM_END;
}

void *pwm_cluster_allocate(size_t size) {
    void *mem = m_malloc_maybe(size);
    if (mem != NULL) {
        if (in_sram(mem)) {
            return mem;
        }
        m_free(mem);
    }
    return __real_malloc(size);
}

void pwm_cluster_deallocate(void *mem) {
    if (mem == NULL) {
        return;
    }
    if (gc_nbytes(mem) > 0) {
        m_free(mem);
    } else {
        __real_free(mem);
    }
}
