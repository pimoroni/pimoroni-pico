#include "profiler.hpp"

namespace pimoroni {
    void Probe::reset() {
        total_us = 0;
        exec_count = 0;
    }

    uint32_t Probe::get_avg_runtime() {
        return total_us / exec_count;
    }

    uint32_t Probe::get_exec_count() {
        return exec_count;
    }

    void Probe::enter() {
        t_start = get_absolute_time();
    }

    void Probe::exit() {
        total_us += absolute_time_diff_us(t_start, get_absolute_time());
        exec_count++;
    }

    namespace Profiler {
        Probe* probes[255];
        uint16_t probe_count = 0;

        Probe* add_probe(const char *name) {
            if(probe_count >= 256) return nullptr;
            Probe *new_probe = new Probe(name);
            probes[probe_count] = new_probe;
            probe_count++;
            return new_probe;
        }

        Probe** get_probes() {
            return probes;
        }

        uint16_t count() {
            return probe_count;
        }
    }
}