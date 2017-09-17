#include "texture.hh"
#include "utils.hh"
#include "image.hh"

#include <GL/glew.h>

texture::texture()
  : _id(0)
  , _image(nullptr)
  , fail_reason("") {
}

texture::~texture() {
  if (_image)
    delete _image;
}

unsigned int texture::get_id() const {
  return _id;
}

void texture::set_image(image* img) {
  if (_image)
    delete _image;
  _image = img;
}

bool texture::upload_texture() {
  if (!_image) {
    fail_reason = "no image in texture";
    return false;
  }

  if (!_id)
    glGenTextures(1, &_id);

  GLuint format;
  switch (_image->get_image_type()) {
    case image_type::rgb:       format = GL_RGB;   break;
    case image_type::rgba:      format = GL_RGBA;  break;
    case image_type::grayscale: format = GL_ALPHA; break;
    default:                    format = GL_RGB;   break;
  }

  glBindTexture(GL_TEXTURE_2D, _id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, _image->get_width()
      , _image->get_height(), 0, format, GL_UNSIGNED_BYTE, _image->get_pixels());

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if (!_id) {
    fail_reason = "failed to upload texture";
    return false;
  }

  return true;
}

