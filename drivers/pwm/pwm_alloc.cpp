#include "pwm_alloc.hpp"
#include <cstdlib>

// Default block source, taking the C heap. The SDK wraps malloc and panics on failure
// where the callers report it, so these call the unwrapped functions; those skip the
// wrapper's cross-core mutex, so cluster init and destruction must not run concurrently
// with another core's allocations.
extern "C" void *__real_malloc(size_t size);
extern "C" void __real_free(void *mem);

extern "C" __attribute__((weak)) void* pwm_allocate(size_t size) {
  return __real_malloc(size);
}

extern "C" __attribute__((weak)) void pwm_deallocate(void* mem) {
  __real_free(mem);
}
