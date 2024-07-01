#include "qformat.hpp"

#include <cmath>

namespace ut = utilities;

ut::q16_16 ut::floatToQ16_16(float value) {
  return static_cast<q16_16>(std::round(value * (1 << 16)));
}

float ut::q16_16ToFloat(ut::q16_16 value) {
  return static_cast<float>(value * pow(2, -16));
}