#include "audio_amp.hpp"

namespace pimoroni {

  static const std::string NAME = "Audio Amp";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | FLOAT | 0     | 1     | 1     | [Proposed] Audio Amp |                             |
  bool AudioAmpModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    // return adc_level == ADC_FLOAT && slow1 == LOW && slow2 == HIGH && slow3 == HIGH;
    return adc_level == ADC_FLOAT && slow1 == HIGH && slow2 == HIGH && slow3 == HIGH;
  }

}
