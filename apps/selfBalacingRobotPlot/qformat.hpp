#pragma once

#include <cstdint>
#include <cmath>

namespace utilities {

typedef int32_t q16_16;

q16_16 floatToQ16_16(float value);
float q16_16ToFloat(q16_16 value);

}