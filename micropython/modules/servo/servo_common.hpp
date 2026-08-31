#pragma once

// ServoCluster hands out and accepts Calibration instances, so both translation
// units need the object layout and the type.
#include "drivers/servo/servo.hpp"

extern "C" {
#include "servo.h"

typedef struct _Calibration_obj_t {
    mp_obj_base_t base;
    servo::Calibration *calibration;
} _Calibtration_obj_t;
}
