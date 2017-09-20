#include "camera.hh"
#include "utils.hh"

camera::camera()
  : pitch(0)
  , yaw(0)
  , roll(0)
  , pos(glm::vec3(0, 0, 0))
  , vel(glm::vec3(0, 0, 0))
  , speed(150) {
}

const float sensitivity = 3.f, m_yaw = 0.022, m_pitch = 0.022
, pitch_max = 89.976f; // carefully choosen constant so as not to cause bugs
void camera::update_view_angles(float xrel, float yrel) {
  yaw += xrel * sensitivity * m_yaw;
  pitch -= yrel * sensitivity * m_pitch;

  if (yaw >= 360.f)
    yaw -= 360.f;
  if (yaw < 0.f)
    yaw += 360.f;

  if (pitch > pitch_max)
    pitch = pitch_max;
  if (pitch < -pitch_max)
    pitch = -pitch_max;
}

void camera::update_position(double dt, int move, int strafe) {
  const float pitch_rad = glm::radians(pitch), yaw_rad = glm::radians(yaw)
    , perp_yaw = yaw_rad + static_cast<float>(M_PI_2);
  float dist_move = speed * static_cast<float>(move * dt)
    , dist_strafe = speed * static_cast<float>(strafe * dt);
  if (std::abs(move) == std::abs(strafe)) {
    dist_move /= sqrtf(2.f);
    dist_strafe /= sqrtf(2.f);
  }
  pos.x += cosf(yaw_rad) * cosf(pitch_rad) * dist_move
    + cosf(perp_yaw) * dist_strafe;
  pos.y += sinf(pitch_rad) * dist_move;
  pos.z += sinf(yaw_rad) * cosf(pitch_rad) * dist_move
    + sinf(perp_yaw) * dist_strafe;
}

glm::vec3 camera::compute_view_dir() const {
  const float pitch_rad = glm::radians(pitch), yaw_rad = glm::radians(yaw);
  return glm::vec3(cos(yaw_rad) * cos(pitch_rad), sin(pitch_rad)
      , sin(yaw_rad) * cos(pitch_rad));
}

glm::mat4 camera::compute_view_mat() const {
  glm::vec3 look_at = pos + compute_view_dir();
  return glm::lookAt(pos, look_at, glm::vec3(0, 1, 0));
}

