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
#define WAKEUP_HAS_RTC (0)
#endif

#ifndef WAKEUP_HAS_SHIFT_REGISTER
#define WAKEUP_HAS_SHIFT_REGISTER (0)
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

// Pins to toggle on wakeup
#ifndef WAKEUP_SHIFT_REG_PIN_MASK
#define WAKEUP_SHIFT_REG_PIN_MASK ((0b1 << 8) | (0b1 << 9) | (0b1 << 10))
#endif

// Direction
#ifndef WAKEUP_SHIFT_REG_PIN_DIR
#define WAKEUP_SHIFT_REG_PIN_DIR ((0b1 << 8) | (0b1 << 9))
#endif

// Value
#ifndef WAKEUP_SHIFT_REG_PIN_VALUE
#define WAKEUP_SHIFT_REG_PIN_VALUE ((0b1 << 9))
#endif

// Clock
#ifndef WAKEUP_SHIFT_REG_CLK
#define WAKEUP_SHIFT_REG_CLK (8)
#endif

// Data
#ifndef WAKEUP_SHIFT_REG_DATA
#define WAKEUP_SHIFT_REG_DATA (10)
#endif