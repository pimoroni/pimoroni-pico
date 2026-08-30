#include "profiler.hpp"
#include <cstring>

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

        void reset() {
            Probe *p = probes[0];
            for(auto x = 0u; x < probe_count; x++) {
                p->reset();
                p++;
            }
        }

        Probe* get_probe(const char *name) {
            Probe *p = probes[0];
            for(auto x = 0u; x < probe_count; x++) {
                if (strncmp(name, p->name.data(), p->name.length()) == 0) {
                    return p;
                }
                p++;
            }
            return nullptr;
        }

        Probe** get_probes() {
            return probes;
        }

        uint16_t count() {
            return probe_count;
        }
    }
}

extern "C" {
#include "profiler.h"

mp_obj_t profiler_reset() {
    pimoroni::Profiler::reset();

    return mp_const_none;
}

mp_obj_t profiler_get_probe(mp_obj_t name) {
    if(mp_obj_is_str_or_bytes(name)) {
        GET_STR_DATA_LEN(name, str, str_len);
        pimoroni::Probe *probe = pimoroni::Profiler::get_probe((const char *)str);

        mp_obj_t tuple_probe[3] = {
            mp_obj_new_str(probe->name.data(), probe->name.length()),
            mp_obj_new_int(probe->get_avg_runtime()),
            mp_obj_new_int(probe->get_exec_count())
        };

        return mp_obj_new_tuple(3, tuple_probe);
    }
    return mp_const_none;
}

mp_obj_t profiler_get_probes() {
    pimoroni::Probe* probe = pimoroni::Profiler::get_probes()[0];
    uint16_t count = pimoroni::Profiler::count();

    mp_obj_t list_probes[count];

    for(auto x = 0u; x < count; x++){
        //mp_printf(&mp_plat_print, "Probe: %s\n", probe->name.data());
        //mp_printf(&mp_plat_print, "  Avg (us): %lu\n", probe->get_avg_runtime());
        //mp_printf(&mp_plat_print, "  Runs: %lu\n", probe->get_exec_count());

        mp_obj_t tuple_probe[3] = {
            mp_obj_new_str(probe->name.data(), probe->name.length()),
            mp_obj_new_int(probe->get_avg_runtime()),
            mp_obj_new_int(probe->get_exec_count())
        };

        list_probes[x] = mp_obj_new_tuple(3, tuple_probe);
        probe++;
    }
    return mp_obj_new_list(count, list_probes);
}

}