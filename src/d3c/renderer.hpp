#pragma once

#include <string>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "../engine/ogl.hh"
#include "../engine/texture.hh"
#include "../engine/camera.hh"

class Batch;

class Renderer {
  std::map<std::string, texture*> m_texture_map;

  glm::mat4 _projection, _view;
  float _viewport_width, _viewport_height;

  shader_program _sp;
  unsigned int _mvp_unif, _texture_sampler_unif;
public:
  Renderer();
  ~Renderer();

  texture* get_texture_from_file(const std::string & name);
  texture* create_texture(const std::string & name);
  void upload_textures();

  bool project(const glm::vec3 &vec, int &x, int &y);

  const glm::mat4 & get_modelview_matrix()  { return _view; }
  const glm::mat4 & get_projection_matrix() { return _projection; }

  void set_viewport(int left, int top, int width, int height);
  void set_renderport(const int left, const int top, const int width
      , const int height);
  void set_view(camera* cam);

  unsigned int vertex_pos_attr, texture_coord_attr, vertex_normal_attr;
};

extern Renderer* renderer;

