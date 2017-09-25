#include "renderer.hpp"
#include "model/batch.hpp"
#include "../engine/screen.hh"
#include "../engine/shaders.hh"
#include "../engine/math.hh"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

void Renderer::_update_frustum() {
  _frustum_planes[frustum_plane_right] = Plane(glm::vec3(_mvp[0][3] - _mvp[0][0]
        , _mvp[1][3] - _mvp[1][0], _mvp[2][3] - _mvp[2][0])
      , _mvp[3][3] - _mvp[3][0]);

  _frustum_planes[frustum_plane_left] = Plane(glm::vec3(_mvp[0][3] + _mvp[0][0]
        , _mvp[1][3] + _mvp[1][0], _mvp[2][3] + _mvp[2][0])
      , _mvp[3][3] + _mvp[3][0]);

  _frustum_planes[frustum_plane_bottom] = Plane(glm::vec3(_mvp[0][3] + _mvp[0][1]
        , _mvp[1][3] + _mvp[1][1], _mvp[2][3] + _mvp[2][1])
      , _mvp[3][3] + _mvp[3][1]);

  _frustum_planes[frustum_plane_top] = Plane(glm::vec3(_mvp[0][3] - _mvp[0][1]
        , _mvp[1][3] - _mvp[1][1], _mvp[2][3] - _mvp[2][1])
      , _mvp[3][3] - _mvp[3][1]);

  _frustum_planes[frustum_plane_back] = Plane(glm::vec3(_mvp[0][3] - _mvp[0][2]
        , _mvp[1][3] - _mvp[1][2], _mvp[2][3] - _mvp[2][2])
      , _mvp[3][3] - _mvp[3][2]);

  _frustum_planes[frustum_plane_front] = Plane(glm::vec3(_mvp[0][3] + _mvp[0][2]
        , _mvp[1][3] + _mvp[1][2], _mvp[2][3] + _mvp[2][2])
      , _mvp[3][3] + _mvp[3][2]);

  for (int i = 0; i < 6; ++i)
    _frustum_planes[i].normalize();
}

Renderer::Renderer()
  : _sp(shaders::map_vert, shaders::map_frag) {
  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_SCISSOR_TEST);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  _sp.use_this_prog();
  vertex_pos_attr = _sp.bind_attrib("vertex_pos");
  texture_coord_attr = _sp.bind_attrib("texture_coord");
  vertex_normal_attr = _sp.bind_attrib("vertex_normal");
  _mvp_unif = _sp.bind_uniform("mvp");
  _texture_sampler_unif = _sp.bind_uniform("texture_sampler");
}

Renderer::~Renderer() {
  for (auto it = m_texture_map.begin(); it != m_texture_map.end(); ++it)
    delete it->second;
}

texture* Renderer::get_texture_from_file(const std::string &name) {
  if (m_texture_map.find(name) != m_texture_map.end())
    return m_texture_map[name];
  image *img = new image(name);

  texture *tex = create_texture(name);
  tex->set_image(img);

  return tex;
}

texture* Renderer::create_texture(const std::string &name) {
  texture *tex = new texture();
  m_texture_map[name] = tex;
  return tex;
}

void Renderer::upload_textures() {
  for (auto it = m_texture_map.begin(); it != m_texture_map.end(); ++it)
    if (!it->second->upload_texture())
      warning_ln("failed to upload texture \"%s\": %s", it->first.c_str()
          , it->second->fail_reason.c_str());
}

bool Renderer::project(const glm::vec3 &vec, int &x, int &y) {
  glm::vec4 out = _mvp * glm::vec4(vec, 1.f);

  if (out.w <= 0.f) {
    x = y = 0;
    return false;
  }

  out /= out.w;

  out.x = out.x * 0.5f + 0.5f;
  out.y = out.y * 0.5f + 0.5f;

  x = out.x * _viewport_width;
  y = out.y * _viewport_height;

  return true;
}

void Renderer::set_viewport(int left, int top, int width, int height) {
  glViewport(left, top, width, height);
  _viewport_width = width;
  _viewport_height = height;

  _projection = glm::perspective(glm::radians(45.f)
      , (float)g_screen->get_window_width()
        / (float)g_screen->get_window_height()
      , 0.1f, 20000.f);
}

void Renderer::set_view(const camera *cam) {
  _view = cam->compute_view_mat();
  _mvp = _projection * _view;

  _update_frustum();

  _sp.use_this_prog();
  glUniformMatrix4fv(_mvp_unif, 1, GL_FALSE, glm::value_ptr(_mvp));
  glUniform1i(_texture_sampler_unif, 0);
}

