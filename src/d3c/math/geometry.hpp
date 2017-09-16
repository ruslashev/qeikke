#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

struct Plane {
  Plane() {}
  Plane(const glm::vec3 &n_normal, float n_d) : normal(n_normal), d(n_d) {}

  ~Plane() {}

  // vector algebra
  inline void normalize() {
    float inv_length = 1.0f / normal.length();
    normal *= inv_length;
    d *= inv_length;
  }

  inline bool is_in_front(const glm::vec3 &point) const {
    return (glm::dot(point, normal) + d > 0);
  }

  inline float distance(const glm::vec3 &point) {
    return glm::dot(point, normal) - d;
  }

  glm::vec3 normal;
  float d;
};

