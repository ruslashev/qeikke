#pragma once

#include <string>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "../engine/ogl.hh"
#include "../engine/texture.hh"
#include "../engine/camera.hh"
#include "math/geometry.hpp"

enum frustum_plane {
  frustum_plane_front  = 0,
  frustum_plane_left   = 1,
  frustum_plane_top    = 2,
  frustum_plane_right  = 3,
  frustum_plane_bottom = 4,
  frustum_plane_back   = 5,
};

class Renderer {
  std::map<std::string, texture*> m_texture_map;
  glm::mat4 _projection, _view, _mvp;
  float _viewport_width, _viewport_height;
  shader_program _sp;
  unsigned int _mvp_unif, _texture_sampler_unif;

  void _update_frustum();
  Plane _frustum_planes[6];
public:
  unsigned int vertex_pos_attr, texture_coord_attr, vertex_normal_attr;

  Renderer();
  ~Renderer();
  texture* get_texture_from_file(const std::string &name);
  texture* create_texture(const std::string &name);
  void upload_textures();
  bool project(const glm::vec3 &vec, int &x, int &y);
  void set_viewport(int left, int top, int width, int height);
  void set_view(const camera *cam);
  const Plane& get_frustum_plane(int idx) const { return _frustum_planes[idx]; }
};

extern Renderer* renderer;

