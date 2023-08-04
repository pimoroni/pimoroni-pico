#include "proto.hpp"

namespace pimoroni {

  const std::string ProtoPotModule::NAME = "Proto Potentiometer";

  // | ADC1  | SLOW1 | SLOW2 | SLOW3 | Module               | Condition (if any)          |
  // |-------|-------|-------|-------|----------------------|-----------------------------|
  // | LOW   | 1     | 1     | 0     | Proto Potentiometer  | Pot in low position         |
  // | FLOAT | 1     | 1     | 0     | Proto Potentiometer  | Pot in middle position      |
  // | HIGH  | 1     | 1     | 0     | Proto Potentiometer  | Pot in high position        |
  bool ProtoPotModule::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == HIGH && slow3 == LOW;
  }

  ProtoPotModule::ProtoPotModule() :
    YukonModule() {
  }

  ProtoPotModule::~ProtoPotModule() {
  }

  std::string ProtoPotModule::name() {
    return ProtoPotModule::NAME;
  }

  float ProtoPotModule::read() {
    return __read_adc1() / 3.3f;
  }


  const std::string ProtoPotModule2::NAME = "Proto Potentiometer 2";

  bool ProtoPotModule2::is_module(uint adc_level, bool slow1, bool slow2, bool slow3) {
    return slow1 == HIGH && slow2 == HIGH && slow3 == LOW;
  }

  ProtoPotModule2::ProtoPotModule2(float pot_resistance) :
    YukonModule() {

    // Normalise the pull-up resistance
    float pu = PULLUP / pot_resistance;

    // Pre-calculate values that are independent of the adc reading
    sum = pu + 1;
    const_a = (4 * pu) + 1;
    const_b = (6 * pu) + 2;
    const_c = (pu * pu) + (2 * pu) + 1;
  }

  ProtoPotModule2::~ProtoPotModule2() {
  }

  std::string ProtoPotModule2::name() {
    return ProtoPotModule2::NAME;
  }

  // ADC2 has a pull-up connected to simplify its use with modules that feature an onboard thermistor.
  // Unfortunately, when connecting up a potentiometer, creating the below circuit, this has the
  // effect of making the output non-linear.
  //
  // Vin ---------------
  //           |       |
  //          | |     | |
  //       rt | |     | | rp
  //          | |     | |
  //           |       |
  //           --------------- Vout
  //           |
  //          | |
  //       rb | |
  //          | |
  //           |
  // Gnd -------
  //
  // Where rt is the top section of the potentiometer, rb is the bottom
  // section of the potentiometer, and rp is the onboard pull-up. Also,
  // The full resistance of the pot, R = rt + rb
  //
  // Below is the equation for calculating the output given a normalised input (e.g. 1V)
  //
  // o =                 rb
  //     ---------------------------------
  //      /             1               |
  //      | ---------------------- + rb |
  //      |  /    1   \   /  1  \       |
  //      |  | ------ | + | --- |       |
  //      \  \ R - rb /   \  rp /       /
  //
  // We want to calculate the inverse of this though... which is this magic equation:
  // https://www.symbolab.com/solver/equation-calculator/o%20%3D%20r%20%2F%20%5Cleft(%201%2F%20%5Cleft(1%2F%5Cleft(10000-r%5Cright)%2B1%2F5100%5Cright)%20%2B%20r%5Cright)?or=input
  // where r is rb, and R and rp are set to 10k and 5.1k, respectively.
  //
  // This can be simplified by normalising the resistor values, by R, giving:
  // https://www.symbolab.com/solver/equation-calculator/o%20%3D%20r%20%2F%20%5Cleft(%201%2F%20%5Cleft(1%2F%5Cleft(1-r%5Cright)%2B1%2F0.51%5Cright)%20%2B%20r%5Cright)?or=input
  float ProtoPotModule2::read() {
    float out = __read_adc2() / 3.3f;
    return (-out + sum - sqrtf((const_a * (out * out)) - (const_b * out) + const_c)) / (2 * (-out + 1));
  }

}
