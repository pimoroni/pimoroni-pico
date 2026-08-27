#include <stdlib.h>

// libstdc++'s default handler prints the demangled type name of an uncaught
// exception, which links the whole C++ demangler - around 42KB. Everything here
// is built -fno-exceptions, so nothing can throw and it is unreachable weight.
namespace __gnu_cxx {
void __verbose_terminate_handler() {
    abort();
}
}
