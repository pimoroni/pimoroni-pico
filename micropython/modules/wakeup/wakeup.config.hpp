#include "hardware/i2c.h"

// Pins to toggle on wakeup
#ifndef WAKEUP_PIN_MASK
#define WAKEUP_PIN_MASK ((0b1 << 2) | (0b1 << 6))
#endif

// Direction
#ifndef WAKEUP_PIN_DIR
#define WAKEUP_PIN_DIR ((0b1 << 2) | (0b1 << 6))
#endif

// Value
#ifndef WAKEUP_PIN_VALUE
#define WAKEUP_PIN_VALUE ((0b1 << 2) | (0b1 << 6))
#endif

#ifndef WAKEUP_HAS_RTC
#define WAKEUP_HAS_RTC (1)
#endif

#ifndef WAKEUP_RTC_SDA
#define WAKEUP_RTC_SDA (4)
#endif

#ifndef WAKEUP_RTC_SCL
#define WAKEUP_RTC_SCL (5)
#endif

#ifndef WAKEUP_RTC_I2C_ADDR
#define WAKEUP_RTC_I2C_ADDR 0x51
#endif

#ifndef WAKEUP_RTC_I2C_INST
#define WAKEUP_RTC_I2C_INST i2c0
#endif