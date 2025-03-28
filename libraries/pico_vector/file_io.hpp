#pragma once
#include <string_view>
#include <cstdint>

class file_io {
private:
    void *state;
    size_t filesize = 0;

public:
    file_io(std::string_view path);
    ~file_io();
    size_t seek(size_t pos);
    size_t read(void *buf, size_t len);
    size_t tell();
    bool fail();
};