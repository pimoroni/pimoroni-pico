#include "lwmem/lwmem.hpp"

class TestClass
{
public:
  TestClass(uint32_t uVal)
  : m_uVal(uVal) 
  {};

  ~TestClass(void)
  {
  }

  void *operator new(size_t size)
  {
    void * p = lwmem_malloc(size);
    return p;
  }

  void operator delete(void *ptr)
  {
    lwmem_free(ptr);
  }
private:
  uint32_t m_uVal;
};