#include "pwm_alloc.hpp"
#include <cstdlib>

// Default block source, taking the C heap. The SDK's malloc wrapper panics on failure, so
// these call the unwrapped functions, which skip its cross-core mutex. A cluster or a
// calibration must not be created, copied or destroyed while another core allocates.
extern "C" void *__real_malloc(size_t size);
extern "C" void __real_free(void *mem);

extern "C" __attribute__((weak)) void* pwm_allocate(size_t size) {
  return __real_malloc(size);
}

extern "C" __attribute__((weak)) void pwm_deallocate(void* mem) {
  __real_free(mem);
}
