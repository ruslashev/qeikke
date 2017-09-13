#include "texture.hpp"
#include "image.hpp"
#include "../misc/log.hpp"
#include "../../engine/utils.hh"

#include <GL/glew.h>

Texture::Texture(const std::string& name) {
  log_init_multiple << "Initialize Texture: " << name << endl;
  m_name = name;

  _id = 0;
}

Texture::~Texture() {
  clear();
}

void Texture::set_image(Image* image) {
  clear();
  m_images.push_back(image);
}

void Texture::set_image(unsigned int index, Image* image) {
  if(index >= m_images.size()) {
    log_warning << "Illegal argument to Texture::set_image(): " << index << endl;
    return;
  }
  if(m_images[index] != NULL) delete m_images[index];
  m_images[index] = image;
}

void Texture::set_num_images(int count) {
  clear();
  m_images.resize(count);

  for (int i = 0; i < count; i++)
    m_images[i] = NULL;
}

void Texture::clear() {
  if (!m_images.size())
    return;
  log_init_multiple << "Free all images of texture " << m_name << endl;

  int count = m_images.size();

  for (int i=0; i<count; i++)
    if (m_images[i] != NULL)
      delete m_images[i];
  m_images.resize(0);
}

bool Texture::upload_texture() {
  log_function_multiple << "Upload texture " << m_name << endl;
  int count = m_images.size();

  if(!count) {
    log_debug << "No images in texture" << endl;
    log_error << "Texture binded unsuccessful" << endl;
    return false;
  }

  if (!_id)
    glGenTextures(1, &_id);

  GLuint format;
  switch (m_images[0]->get_image_type()) {
    case Image::RGB:       format = GL_RGB;   break;
    case Image::RGBA:      format = GL_RGBA;  break;
    case Image::GREYSCALE: format = GL_ALPHA; break;
    default:               format = GL_RGB;   break;
  }

  glBindTexture(GL_TEXTURE_2D, _id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, m_images[0]->get_image_width()
      , m_images[0]->get_image_height(), 0, format, GL_UNSIGNED_BYTE
      , m_images[0]->get_pixels());

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if (!_id)
    die("failed to upload texture");

  return !!_id;
}

