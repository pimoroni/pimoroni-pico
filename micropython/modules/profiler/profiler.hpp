#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include "pico/stdlib.h"

namespace pimoroni {

    class Probe {
        private:
            uint64_t total_us;
            uint32_t exec_count;
            absolute_time_t t_start;

        public:
            std::string_view name;

            Probe(const char* name) : total_us(0), exec_count(0), name(name) {
            }

            void reset();

            uint32_t get_avg_runtime();

            uint32_t get_exec_count();

            void __not_in_flash_func(enter)();

            void __not_in_flash_func(exit)();
    };

    namespace Profiler {
        Probe *add_probe(const char *name);
        uint16_t count();
        Probe** get_probes();
    };
}