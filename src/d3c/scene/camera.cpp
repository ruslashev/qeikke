#include "camera.hpp"
#include "../misc/log.hpp"

#include "../renderer.hpp"
#include "../../engine/screen.hh"

//==============================================================================
//  Camera::Camera
//==============================================================================
Camera::Camera() {
  m_controlled = false;
  set_perspective(45.0f, 0.15f ,50000.0f);
  m_type = CAM_3D;

  log_init << "Camera initialization" << endl;

  m_control_fps = new Camera_controller_fps();
  set_controller( m_control_fps );
  set_controlled(true);

  pitch = yaw = 0;
}

Camera::~Camera() {
  delete m_control_fps;
}

const float sensitivity = 3.f, m_yaw = 0.022, m_pitch = 0.022
, pitch_max = 89.998f;
void Camera::update_view_angles(float xrel, float yrel) {
  float yaw = xrel * sensitivity * m_yaw;
  float pitch = yrel * sensitivity * m_pitch;

  m_control_fps->set_rotation(deg_to_rad(-yaw), deg_to_rad(-pitch));
}

void Camera::update(int imove, int istrafe, float dt) {
  m_control_fps->update(imove, istrafe, dt);
}

//==============================================================================
//  Camera::set_perspective()
//==============================================================================
void Camera::set_perspective(float fov_y, float near_plane, float far_plane) {
  m_fov_y = fov_y;
  m_near_plane = near_plane;
  m_far_plane = far_plane;
}

//==============================================================================
//  Camera::get_perspective()
//==============================================================================
void Camera::get_perspective(float &fov_y, float &near_plane, float &far_plane) {
  fov_y = m_fov_y;
  near_plane = m_near_plane;
  far_plane = m_far_plane;
}


//==============================================================================
//  Camera::update_frustum()
//==============================================================================
void Camera::update_frustum() {
  Matrix4x4f clip = renderer->get_projection_matrix() * renderer->get_modelview_matrix();

  // This will extract the RIGHT side of the frustum
  m_frustum_planes[FRUSTUM_RIGHT ] = Plane(
      Vector3f( clip[ 3] - clip[ 0], clip[ 7] - clip[ 4], clip[11] - clip[ 8]), clip[15] - clip[12]);

  // This will extract the LEFT side of the frustum
  m_frustum_planes[FRUSTUM_LEFT  ] = Plane(
      Vector3f( clip[ 3] + clip[ 0], clip[ 7] + clip[ 4], clip[11] + clip[ 8]), clip[15] + clip[12]);

  // This will extract the BOTTOM side of the frustum
  m_frustum_planes[FRUSTUM_BOTTOM] = Plane(
      Vector3f( clip[ 3] + clip[ 1], clip[ 7] + clip[ 5], clip[11] + clip[ 9]), clip[15] + clip[13]);

  // This will extract the TOP side of the frustum
  m_frustum_planes[FRUSTUM_TOP   ] = Plane(
      Vector3f( clip[ 3] - clip[ 1], clip[ 7] - clip[ 5], clip[11] - clip[ 9]), clip[15] - clip[13]);

  // This will extract the BACK side of the frustum
  m_frustum_planes[FRUSTUM_BACK  ] = Plane(
      Vector3f( clip[ 3] - clip[ 2], clip[ 7] - clip[ 6], clip[11] - clip[10]), clip[15] - clip[14]);

  // This will extract the FRONT side of the frustum
  m_frustum_planes[FRUSTUM_FRONT ] = Plane(
      Vector3f( clip[ 3] + clip[ 2], clip[ 7] + clip[ 6], clip[11] + clip[10]), clip[15] + clip[14]);

  // And normalize..
  for(int i=0; i<FRUSTUM_PLANES; ++i) {
    m_frustum_planes[i].normalize();
  }
}

//==============================================================================
//  Camera_controller_fps::Camera_controller_fps()
// remember, z is up
//==============================================================================
Camera_controller_fps::Camera_controller_fps() {
  look_at( Vector3f(0.0, 0.0, 0.0),
      Vector3f(0.0, 1.0, 0.5),
      Vector3f(0.0, 0.0, 1.0) );

  m_speed = 150.0f;
}

//==============================================================================
//  Camera_controller_fps::set_rotation()
//==============================================================================
void Camera_controller_fps::set_rotation(float angle_z, float angle_y) {
  // Todo: if the current rotation (in radians) is greater than 1.0,
  // we want to cap it.

  Vector3f axis = cross(m_view - m_position, m_up);
  axis.normalize();

  set_rotation(angle_y, axis);
  // Rotate around the y axis no matter what the current_rot_x is
  set_rotation(angle_z, Vector3f(0, 0, 1) );
}

//==============================================================================
//  Camera_controller_fps::set_rotation()
//==============================================================================
void Camera_controller_fps::set_rotation(float angle, Vector3f axis) {
  Vector3f new_view;
  // Get the view vector (The direction we are facing)
  Vector3f view = m_view - m_position;

  // build rotation matrix
  Matrix3x3f rotation;
  rotation.set_rotation_axis( angle, axis );

  new_view = rotation * view;
  m_up = rotation * m_up;

  // Now we just add the newly rotated vector to our position to set
  // our new rotated view of our Camera.
  m_view = m_position + new_view;

  // update strafe
  m_strafe = cross(new_view.get_normalized(), m_up).get_normalized();

  look_at( m_position, m_view, m_up);
}

//==============================================================================
//  Camera_controller_fps::strafe()
//==============================================================================
void Camera_controller_fps::strafe(float speed) {
  // Add the strafe vector to our position
  m_position.x += m_strafe.x * speed;
  m_position.y += m_strafe.y * speed;

  // Add the strafe vector to our view
  m_view.x += m_strafe.x * speed;
  m_view.y += m_strafe.y * speed;
}

//==============================================================================
//  Camera_controller_fps::move()
//==============================================================================
void Camera_controller_fps::move(float speed) {
  // Get the current view vector (the direction we are looking)
  Vector3f movement = m_view - m_position;
  movement.normalize();

  movement *= speed;

  m_position += movement;
  m_view += movement;
}

void Camera_controller_fps::update(int imove, int istrafe, float dt) {
  float m_speed = 150.f, speed = m_speed * dt
    , speeds2 = fast_inv_sqrt(2) * m_speed * dt;
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

