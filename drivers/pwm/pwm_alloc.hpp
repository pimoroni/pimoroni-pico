#pragma once

#include <cstddef>

// The cluster and calibration classes claim and release their blocks through these, so a
// port can supply memory from its own heap. PWMCluster's block is a DMA source, which may
// be PSRAM as well as SRAM. The defaults, defined weak in pwm_alloc.cpp, take the C heap.
// Returns nullptr on failure; callers report that.
extern "C" void* pwm_allocate(size_t size);
extern "C" void pwm_deallocate(void* mem);
