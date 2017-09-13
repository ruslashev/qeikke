#pragma once

#include "../math/geometry.hpp"
#include <glm/gtc/matrix_transform.hpp>

enum Camera_type {
  CAM_3D = 0,
  CAM_2D = 1
};

enum Frustum_plane {
  FRUSTUM_FRONT		= 0,
  FRUSTUM_LEFT		= 1,
  FRUSTUM_TOP			= 2,
  FRUSTUM_RIGHT		= 3,
  FRUSTUM_BOTTOM		= 4,
  FRUSTUM_BACK		= 5,
  FRUSTUM_PLANES		= 6
};

class Camera {
  float pitch, yaw;

  float m_near_plane;
  float m_far_plane;
  float m_fov_y;

  Camera_type m_type;
  Plane m_frustum_planes[FRUSTUM_PLANES];

  glm::vec3 m_position;
  glm::mat3 m_orientation;

  float m_speed;

  glm::vec3 m_view;
  glm::vec3 m_strafe;
  glm::vec3 m_up;
public:
  Camera();
  ~Camera();

  void set_type(const Camera_type type) { m_type = type; }
  Camera_type get_type() const { return m_type; }

  void set_perspective(float fov_y, float near_plane, float far_plane);
  void get_perspective(float &fov_y, float &near_plane, float &far_plane);

  void update_frustum();
  Plane & get_frustum_plane(int index) const { return (Plane &)m_frustum_planes[index]; }

  void update_view_angles(float xrel, float yrel);
  void update(int imove, int istrafe, float dt);

  inline void set_position (const glm::vec3 &position) {
    look_at( position, m_view + (position - m_position), m_up);
  }
  inline void look_at( const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up) {
    m_orientation = glm::lookAt(eye, center, up);
    m_position = eye;
    m_up = up;
    m_view = center;
  }
  void set_rotation(float angle, glm::vec3 axis);
  void set_rotation(float angle_x, float angle_z);
  void strafe(float speed);
  void move(float speed);

  virtual void set_orientation(const glm::mat3 & orientation) { m_orientation = orientation; };

  glm::vec3 get_position() const {
    return m_position;
  }

  glm::mat3 get_orientation() const {
    return m_orientation;
  }
};

