#pragma once

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

glm::mat3 set_rotation_axis(const double angle, const glm::vec3 & axis) {
  glm::vec3 u = glm::normalize(axis);

  float sinfAngle = (float)sinf(angle);
  float cosfAngle = (float)cosf(angle);
  float oneMinuscosfAngle = 1.0f - cosfAngle;

  glm::mat3 x;

  x[0][0] = u.x * u.x + cosfAngle*(1-(u.x)*(u.x));
  x[1][0] = u.x * u.y * (oneMinuscosfAngle) - sinfAngle*u.z;
  x[2][0] = u.x * u.z * (oneMinuscosfAngle) + sinfAngle*u.y;

  x[0][1] = u.x * u.y * (oneMinuscosfAngle) + sinfAngle*u.z;
  x[1][1] = u.y * u.y + cosfAngle*(1-(u.y)*(u.y));
  x[2][1] = u.y * u.z * (oneMinuscosfAngle) - sinfAngle*u.x;

  x[0][2] = u.x * u.z * (oneMinuscosfAngle) - sinfAngle*u.y;
  x[1][2] = u.y * u.z * (oneMinuscosfAngle) + sinfAngle*u.x;
  x[2][2] = u.z * u.z + cosfAngle*(1-(u.z)*(u.z));

  return x;
}

