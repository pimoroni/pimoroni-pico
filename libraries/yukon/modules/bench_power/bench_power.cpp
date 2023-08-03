#include "bench_power.hpp"

namespace pimoroni {

  const std::string BenchPowerModule::NAME = "Bench Power";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 0     | 0     | Bench Power          |                             |
  // | FLOAT | 1     | 0     | 0     | Bench Power          | When V+ is discharging      |
  // FLOAT address included as may not be able to rely on the ADC level being low
  bool BenchPowerModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == LOW && slow3 == LOW;
  }

}
