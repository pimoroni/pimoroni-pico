#include <new>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <exception>

typedef unsigned _Unwind_Word __attribute__((__mode__(__word__)));

#ifdef PIMORONI_VERBOSE_TERMINATE_HANDLER
namespace __gnu_cxx
{
    void __verbose_terminate_handler()
    {
    for (;;)
        ;
    }
}

[[noreturn]] void terminate() noexcept
{
    for (;;)
        ;
}

namespace __cxxabiv1
{
std::terminate_handler __terminate_handler = terminate;
}

extern "C" {
void __wrap__ZSt25__throw_bad_function_callv(void) {};
void __wrap__ZSt19__throw_logic_errorPKc(const char *) {};
void __wrap__ZSt20__throw_length_errorPKc(const char *) {};

void __wrap___aeabi_unwind_cpp_pr0(void) {};
void __wrap___aeabi_unwind_cpp_pr1(void) {};
void __wrap___aeabi_unwind_cpp_pr2(void) {};

void * __wrap__malloc_r (struct _reent *, size_t) {return NULL;};
void * __wrap__malloc_av (struct _reent *, size_t) {return NULL;};
void __wrap_qsort(void *base, __SIZE_TYPE__ nmemb, __SIZE_TYPE__ size,
              int (*compar)(const void *, const void *)) {};
}
#endif

enum allocator_mode {
    FIXED_HEAP = 0,
    MICROPYTHON
};

#ifndef CPP_FIXED_HEAP_SIZE
#define CPP_FIXED_HEAP_SIZE 1024u
#endif

static uint32_t alloc_bytes = 0;
static uint32_t alloc_count = 0;
static uint32_t free_count = 0;

static allocator_mode mode = FIXED_HEAP;
static constexpr size_t cpp_heap_size = CPP_FIXED_HEAP_SIZE / 4;
static uint32_t cpp_heap[cpp_heap_size];
static uint32_t ptr = 0;

extern "C" {
#include "cppmem.h"

mp_obj_t cpp_mem_set_allocator_mode(mp_obj_t new_mode) {
    mode = (allocator_mode)mp_obj_get_int(new_mode);
    return mp_const_none;
}

mp_obj_t cpp_mem_get_allocator_mode() {
    return mp_obj_new_int((int)mode);
}

mp_obj_t cpp_mem_get_alloc_bytes() {
    return mp_obj_new_int(alloc_bytes);
}

mp_obj_t cpp_mem_get_alloc_count() {
    return mp_obj_new_int(alloc_count);
}

mp_obj_t cpp_mem_get_free_count() {
    return mp_obj_new_int(free_count);
}

void mpy_free(void *p) {
    return m_free(p);
}

void* mpy_malloc(size_t n) {
    return m_malloc(n);
}

}

std::size_t alloc_size(std::size_t n) {
    std::size_t s = n / 4;
    if(n & 0b11) s++;
    return s;
}

void* stat_new(std::size_t n) {
    if(mode == MICROPYTHON) {
        return mpy_malloc(n);
    }
    std::size_t s = alloc_size(n);
    if(ptr + s > cpp_heap_size) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to allocate %d bytes."), s * 4);
        return nullptr;
    }
    alloc_bytes += s * 4;
    alloc_count ++;
    void *result = &cpp_heap[ptr];
    //memset(result, n, 0);
    ptr += s;
    return result;
    //return std::malloc(n);
}

void stat_free(void *p) {
    if(mode == MICROPYTHON) {
        return mpy_free(p);
    }
    free_count ++;
    //return std::free(p);
    return;
}

void *operator new(std::size_t n) {
    return stat_new(n);
}

void *operator new[](std::size_t n) {
    return stat_new(n);
}

void operator delete(void *p) { stat_free(p); }

void operator delete[](void *p) noexcept { stat_free(p); }

#if __cpp_sized_deallocation

void operator delete(void *p, __unused std::size_t n) noexcept { stat_free(p); }

void operator delete[](void *p, __unused std::size_t n) noexcept { stat_free(p); }

#endif