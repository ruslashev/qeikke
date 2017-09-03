//============================================================================//
// This source file is part of work done by Reinder Nijhoff (reinder@infi.nl) //
// For the latest info, see http://developer.infi.nl                          //
//                                                                            //
// You're free to use the code in any way you like, modified, unmodified or   //
// cut'n'pasted into your own work.                                           //
//                                                                            //
// Part of this source is based on work by:                                   //
//    - Humus (http://esprit.campus.luth.se/~humus/)                          //
//    - Paul Baker (http://www.paulsprojects.net)                             //
//============================================================================//

#include "renderer.hpp"
#include "material/texture.hpp"
#include "material/image.hpp"
#include "misc/log.hpp"
#include "model/batch.hpp"
#include "scene/camera.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>

//==============================================================================
//  Renderer::get_texture_from_file()
//==============================================================================
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

//==============================================================================
//  Renderer::get_texture()
//==============================================================================
Texture* Renderer::get_texture(const std::string & name) {
  if(m_texture_map.find(name) != m_texture_map.end()) {
    log_debug_multiple << name << " found in m_texture_map in Renderer::get_texture() " << endl;
    return m_texture_map[name];
  }
  return create_texture(name);
}

//==============================================================================
//  Renderer::init()
//==============================================================================
void Renderer::init() {
  GLenum err = glewInit();
  if (err != GLEW_OK)
    printf("failed to initialze glew: %s", glewGetErrorString(err));

  glClearColor( 0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
}

//==============================================================================
//  Renderer::close()
//==============================================================================
void Renderer::close() {
}

//==============================================================================
//  Renderer::set_viewport()
//==============================================================================
void Renderer::set_viewport(const int left, const int top, const int width, const int height) {
  glViewport(left, top, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, (float)width/(float)height, 0.15f ,50000.0f);
  set_renderport(left, top, width, height);
}

//==============================================================================
//  Renderer::create_batch()
//==============================================================================
Batch* Renderer::create_batch() {
  Batch* batch = new Batch();

  return (Batch *)batch;
}

//==============================================================================
//  Renderer::create_texture()
//==============================================================================
Texture* Renderer::create_texture(const std::string & name) {
  Texture* texture = new Texture(name);
  m_texture_map[name] = texture;
  return (Texture *)texture;
}

//==============================================================================
//  Renderer::set_renderport()
//==============================================================================
void Renderer::set_renderport(const int left, const int top, const int width, const int height) {
  glScissor(left, top, width, height);
}

//==============================================================================
//  Renderer::set_view()
//==============================================================================
void Renderer::set_view(Camera* camera) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();                                   // Reset The matrix

  static Matrix4x4f mat;
  const Vector3f eye = camera->get_position();
  mat = Matrix4x4f( camera->get_orientation() );

  glMultMatrixf(mat);
  glTranslated(-eye.x,-eye.y,-eye.z );

  // setup matrixes
  glGetFloatv(GL_MODELVIEW_MATRIX, m_modelview_matrix);
  glGetFloatv(GL_PROJECTION_MATRIX, m_projection_matrix);
  glGetIntegerv(GL_VIEWPORT, m_viewport);

  // update frustum of camera
  camera->update_frustum();
}

//==============================================================================
//  Renderer::upload_textures()
//==============================================================================
void Renderer::upload_textures() {
  std::map<std::string, Texture*>::const_iterator it;

  for ( it = m_texture_map.begin(); it != m_texture_map.end(); ++it ) {
    it->second->upload_texture();
  }
}

//==============================================================================
//  Renderer::bind_texture()
//==============================================================================
void Renderer::bind_texture(Texture* texture) {
  glBindTexture(GL_TEXTURE_2D, texture->get_tex_id() );
}

