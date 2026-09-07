#pragma once

#include <cstddef>

// The cluster and calibration classes claim and release their blocks through these, so a
// port can supply memory from its own heap. PWMCluster's block is a DMA source, so the
// memory must be somewhere the DMA reads coherently with the CPU's writes, in practice
// SRAM. The defaults, defined weak in pwm_alloc.cpp, take the C heap, which the
// RP2 ports place in SRAM. Returns nullptr on failure; callers report that.
extern "C" void* pwm_allocate(size_t size);
extern "C" void pwm_deallocate(void* mem);
