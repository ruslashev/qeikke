#include "renderer.hpp"
#include "material/texture.hpp"
#include "material/image.hpp"
#include "misc/log.hpp"
#include "model/batch.hpp"
#include "scene/camera.hpp"

#include <GL/glew.h>
#include <GL/glu.h>

Texture* Renderer::get_texture_from_file(const std::string & name) {
  if(m_texture_map.find(name) != m_texture_map.end()) {
    log_debug_multiple << name << " found in m_texture_map in Renderer::get_texture_from_file() " << endl;
    return m_texture_map[name];
  }
  Image* image = new Image();

  // todo, create an 'file not found' default texture;
  if(!image->load_form_file(name)) {
    log_error << "unable to load image: " << name << endl;
    image->create_empty();
  }

  Texture* texture = create_texture(name);
  texture->set_image(image);

  return texture;
}

Texture* Renderer::get_texture(const std::string & name) {
  if(m_texture_map.find(name) != m_texture_map.end()) {
    log_debug_multiple << name << " found in m_texture_map in Renderer::get_texture() " << endl;
    return m_texture_map[name];
  }
  return create_texture(name);
}

void Renderer::init() {
  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_2D);
}

void Renderer::set_viewport(const int left, const int top, const int width, const int height) {
  glViewport(left, top, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, (float)width/(float)height, 0.15f ,50000.0f);
  set_renderport(left, top, width, height);
}

Batch* Renderer::create_batch() {
  Batch* batch = new Batch();

  return (Batch *)batch;
}

Texture* Renderer::create_texture(const std::string & name) {
  Texture* texture = new Texture(name);
  m_texture_map[name] = texture;
  return (Texture *)texture;
}

void Renderer::set_renderport(const int left, const int top, const int width, const int height) {
  glScissor(left, top, width, height);
}

void Renderer::set_view(Camera* camera) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();                                   // Reset The matrix

  static glm::mat4 mat;
  const glm::vec3 eye = camera->get_position();
  mat = glm::mat4( camera->get_orientation() );

  glMultMatrixf(&mat[0][0]);
  glTranslated(-eye.x,-eye.y,-eye.z );

  // setup matrixes
  glGetFloatv(GL_MODELVIEW_MATRIX, &m_modelview_matrix[0][0]);
  glGetFloatv(GL_PROJECTION_MATRIX, &m_projection_matrix[0][0]);
  glGetIntegerv(GL_VIEWPORT, m_viewport);

  // update frustum of camera
  // camera->update_frustum();
}

void Renderer::upload_textures() {
  for (std::map<std::string, Texture*>::const_iterator it = m_texture_map.begin()
      ; it != m_texture_map.end(); ++it)
    it->second->upload_texture();
}

void Renderer::bind_texture(Texture* texture) {
  glBindTexture(GL_TEXTURE_2D, texture->get_tex_id() );
}

