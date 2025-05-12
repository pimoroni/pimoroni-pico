#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    void *af_tracked_malloc(size_t size);
    void *af_tracked_realloc(void *p, size_t size);
    void af_tracked_free(void *p);

    void *af_malloc(size_t size);
    void *af_realloc(void *p, size_t size);
    void af_free(void *p);
    void af_debug(const char *fmt, ...);
#ifdef __cplusplus
}
#endif