#include "af-file-io.h"
#include "string.h"

static size_t ptr = 0;

void* fileio_open(const char* filename) {
    ptr = 0;
    return NULL;
}

void fileio_close(void* fhandle) {
    ptr = 0;
    return;
}

size_t fileio_read(void* fhandle, void *buf, size_t len) {
    memcpy(buf, fhandle + ptr, len);
    ptr += len;
    return len;
}

int fileio_getc(void* fhandle) {
    uint8_t *f = fhandle;
    int c = f[ptr];
    ptr += 1;
    return c;
}

size_t fileio_tell(void* fhandle) {
    return ptr;
}

size_t fileio_seek(void* fhandle, size_t pos) {
    ptr = pos;
    return ptr;
}