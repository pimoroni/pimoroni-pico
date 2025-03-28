#include <stdlib.h>
#include <stdint.h>

extern "C" {
void* fileio_open(const char* filename);

void fileio_close(void* fhandle);

size_t fileio_read(void* fhandle, void *buf, size_t len);

int fileio_getc(void* fhandle);

size_t fileio_tell(void* fhandle);

size_t fileio_seek(void* fhandle, size_t pos);
}