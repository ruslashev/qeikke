#include "camera.hpp"
#include "../misc/log.hpp"

#include "../renderer.hpp"
#include "../../engine/screen.hh"
#include "../math.hpp"

Camera::Camera() {
  set_perspective(45.0f, 0.15f ,50000.0f);
  m_type = CAM_3D;

  log_init << "Camera initialization" << endl;

  pitch = yaw = 0;

  look_at( glm::vec3(0.0, 0.0, 0.0),
      glm::vec3(0.0, 1.0, 0.5),
      glm::vec3(0.0, 0.0, 1.0) );

  m_speed = 150.0f;
}

Camera::~Camera() {
}

const float sensitivity = 3.f, m_yaw = 0.022, m_pitch = 0.022
, pitch_max = 89.998f;
void Camera::update_view_angles(float xrel, float yrel) {
  float yaw = xrel * sensitivity * m_yaw
    , pitch = yrel * sensitivity * m_pitch;
  set_rotation(glm::radians(-yaw), glm::radians(-pitch));
}

void Camera::update(int imove, int istrafe, float dt) {
  float m_speed = 150.f, speed = m_speed * dt
    , speeds2 = 1.f / sqrtf(2.f) * m_speed * dt;
  if (imove != 0 && istrafe == 0) {
    if (imove > 0)
      move(speed);
    else
      move(-speed);
  } else if (imove == 0 && istrafe != 0) {
    if (istrafe > 0)
      strafe(speed);
    else
      strafe(-speed);
  } else if (imove != 0 && istrafe != 0) {
    if (imove > 0)
      move(speeds2);
    else
      move(-speeds2);
    if (istrafe > 0)
      strafe(speeds2);
    else
      strafe(-speeds2);
  }
}

void Camera::set_perspective(float fov_y, float near_plane, float far_plane) {
  m_fov_y = fov_y;
  m_near_plane = near_plane;
  m_far_plane = far_plane;
}

void Camera::get_perspective(float &fov_y, float &near_plane, float &far_plane) {
  fov_y = m_fov_y;
  near_plane = m_near_plane;
  far_plane = m_far_plane;
}


void Camera::update_frustum() {
#if 0
  Matrix4x4f clip = renderer->get_projection_matrix() * renderer->get_modelview_matrix();

  // This will extract the RIGHT side of the frustum
  m_frustum_planes[FRUSTUM_RIGHT ] = Plane(
      glm::vec3( clip[ 3] - clip[ 0], clip[ 7] - clip[ 4], clip[11] - clip[ 8])
      , clip[15] - clip[12]);

  // This will extract the LEFT side of the frustum
  m_frustum_planes[FRUSTUM_LEFT  ] = Plane(
      glm::vec3( clip[ 3] + clip[ 0], clip[ 7] + clip[ 4], clip[11] + clip[ 8])
      , clip[15] + clip[12]);

  // This will extract the BOTTOM side of the frustum
  m_frustum_planes[FRUSTUM_BOTTOM] = Plane(
      glm::vec3( clip[ 3] + clip[ 1], clip[ 7] + clip[ 5], clip[11] + clip[ 9])
      , clip[15] + clip[13]);

  // This will extract the TOP side of the frustum
  m_frustum_planes[FRUSTUM_TOP   ] = Plane(
      glm::vec3( clip[ 3] - clip[ 1], clip[ 7] - clip[ 5], clip[11] - clip[ 9])
      , clip[15] - clip[13]);

  // This will extract the BACK side of the frustum
  m_frustum_planes[FRUSTUM_BACK  ] = Plane(
      glm::vec3( clip[ 3] - clip[ 2], clip[ 7] - clip[ 6], clip[11] - clip[10])
      , clip[15] - clip[14]);

  // This will extract the FRONT side of the frustum
  m_frustum_planes[FRUSTUM_FRONT ] = Plane(
      glm::vec3( clip[ 3] + clip[ 2], clip[ 7] + clip[ 6], clip[11] + clip[10])
      , clip[15] + clip[14]);

  // And normalize..
  for(int i=0; i<FRUSTUM_PLANES; ++i) {
    m_frustum_planes[i].normalize();
  }
#endif
}

void Camera::set_rotation(float angle_z, float angle_y) {
  // Todo: if the current rotation (in radians) is greater than 1.0,
  // we want to cap it.

  glm::vec3 axis = cross(m_view - m_position, m_up);
  axis = glm::normalize(axis);

  set_rotation(angle_y, axis);
  // Rotate around the y axis no matter what the current_rot_x is
  set_rotation(angle_z, glm::vec3(0, 0, 1) );
}

void Camera::set_rotation(float angle, glm::vec3 axis) {
  glm::vec3 new_view;
  // Get the view vector (The direction we are facing)
  glm::vec3 view = m_view - m_position;

  // build rotation matrix
  glm::mat3 rotation = set_rotation_axis(angle, axis);

  new_view = rotation * view;
  m_up = rotation * m_up;

  // Now we just add the newly rotated vector to our position to set
  // our new rotated view of our Camera.
  m_view = m_position + new_view;

  // update strafe
  m_strafe = glm::normalize(cross(glm::normalize(new_view), m_up));

  look_at( m_position, m_view, m_up);
}

void Camera::strafe(float speed) {
  // Add the strafe vector to our position
  m_position.x += m_strafe.x * speed;
  m_position.y += m_strafe.y * speed;

  // Add the strafe vector to our view
  m_view.x += m_strafe.x * speed;
  m_view.y += m_strafe.y * speed;
}

void Camera::move(float speed) {
  // Get the current view vector (the direction we are looking)
  glm::vec3 movement = glm::normalize(m_view - m_position);

  movement *= speed;

  m_position += movement;
  m_view += movement;
}

