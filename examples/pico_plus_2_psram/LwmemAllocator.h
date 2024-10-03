#include "lwmem/lwmem.hpp"

template<class T>
struct LwmemAllocator
{
    typedef T value_type;
 
    LwmemAllocator() = default;
 
    template<class U>
    constexpr LwmemAllocator(const LwmemAllocator <U>&) noexcept {}
 
    [[nodiscard]] T* allocate(std::size_t n)
    {
        if (auto p = static_cast<T*>(lwmem_malloc(n * sizeof(T))))
          return p;
        else
          return nullptr;
    }
 
    void deallocate(T* p, std::size_t n) noexcept
    {
        lwmem_free(p);
    }
private:
};
 
template<class T, class U>
bool operator==(const LwmemAllocator <T>&, const LwmemAllocator <U>&) { return true; }
 
template<class T, class U>
bool operator!=(const LwmemAllocator <T>&, const LwmemAllocator <U>&) { return false; }
 
