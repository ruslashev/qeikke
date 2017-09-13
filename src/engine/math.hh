#pragma once

#include <cmath>

bool feq(float a, float b, float epsilon = 0.00001f) {
  return std::abs(a - b) < epsilon;
}

