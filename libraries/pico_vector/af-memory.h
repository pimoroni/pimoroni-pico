extern "C" {
    void *af_malloc(size_t size);
    void *af_realloc(void *p, size_t size);
    void af_free(void *p);
}