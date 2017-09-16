#pragma once

#include <string>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "../engine/ogl.hh"
#include "../engine/texture.hh"

class Batch;
class Camera;

class Renderer {
  int m_frame;
  std::map<std::string, texture*> m_texture_map;

  glm::mat4 _projection, _view;
  int m_viewport[4];

  shader_program _sp;
  unsigned int _mvp_unif, _texture_sampler_unif;
public:
  Renderer();

  texture* get_texture_from_file(const std::string & name);
  texture* create_texture(const std::string & name);
  void upload_textures();

  // unproject
  bool project(glm::vec3 vec, int &x, int &y);
  bool project(glm::vec4 vec, int &x, int &y);

  const glm::mat4 & get_modelview_matrix()  { return _view; }
  const glm::mat4 & get_projection_matrix() { return _projection; }

  inline int get_frame() { return m_frame; }
  void next_frame() { m_frame++; }

  void set_viewport(const int left, const int top, const int width
      , const int height);
  void set_renderport(const int left, const int top, const int width
      , const int height);
  void set_view(Camera* camera);

  unsigned int vertex_pos_attr, texture_coord_attr, vertex_normal_attr;
};

extern Renderer* renderer;

