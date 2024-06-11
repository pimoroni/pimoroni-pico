#include "af-memory.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void *af_malloc(size_t size) {
    return malloc(size);
}

void *af_realloc(void *p, size_t size) {
    return realloc(p, size);
}

void af_free(void *p) {
    free(p);
}

void af_debug(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = printf(fmt, ap);
    va_end(ap);
    (void)ret;
}