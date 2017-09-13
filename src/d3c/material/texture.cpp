#include "texture.hpp"
#include "image.hpp"
#include "../misc/log.hpp"

#include <GL/glew.h>

Texture::Texture(const std::string& name) {
  log_init_multiple << "Initialize Texture: " << name << endl;
  m_name = name;

  /* default */
  m_tex_id = 0;
  m_type = BYTE;
  m_dimension = TEX_2D;
  m_use_nearest = false;
  m_use_clamp = false;
}

Texture::~Texture() {
  clear();
}

void Texture::set_image(Image* image) {
  clear();
  m_images.push_back(image);
}

void Texture::set_image(int index, Image* image) {
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

  for(int i=0; i<count; i++) {
    m_images[i] = NULL;
  }
}

void Texture::clear() {
  if(m_images.size()) {
    log_init_multiple << "Free all images of texture " << m_name << endl;

    int count = m_images.size();

    for(int i=0; i<count; i++) {
      if(m_images[i] != NULL) delete m_images[i];
    }
    m_images.resize(0);
  }
}

bool Texture::upload_texture() {
  log_function_multiple << "Upload texture " << m_name << endl;
  int count = m_images.size();

  if(!count) {
    log_debug << "No images in texture" << endl;
    log_error << "Texture binded unsuccessful" << endl;
    return false;
  }

  bool created = false;

  if(!m_tex_id) {
    glGenTextures( 1, &m_tex_id );
  }

  if(m_tex_id) {
    created = true;
  }

  GLuint format;
  GLuint type;
  GLuint dimension;

  switch(m_images[0]->get_image_type() ) {
    case Image::RGB:	   format = GL_RGB;
                         break;
    case Image::RGBA:	   format = GL_RGBA;
                         break;
    case Image::GREYSCALE: format = GL_ALPHA;
                           break;
  }

  switch(m_type) {
    case BYTE:  type = GL_UNSIGNED_BYTE;
                break;
    case FLOAT: type = GL_FLOAT;
                log_error << "Float type image not supported!" << endl;
                break;
  }

  switch(m_dimension) {
    case TEX_1D:   dimension = GL_TEXTURE_1D;
                   log_debug_multiple << "type = GL_TEXTURE_1D" << endl;
                   break;
    case TEX_2D:   dimension = GL_TEXTURE_2D;
                   log_debug_multiple << "type = GL_TEXTURE_2D" << endl;
                   break;
    // case TEX_3D:   dimension = GL_TEXTURE_3D;
    //                log_debug_multiple << "type = GL_TEXTURE_3D" << endl;
    //                break;
    // case TEX_CUBE: if(!GL_ARB_texture_cube_map_supported) {
    //   log_warning << "GL_TEXTURE_CUBE_MAP is not supported!" << endl;
    //   dimension = GL_TEXTURE_2D;
    // } else {
    //   dimension = GL_TEXTURE_CUBE_MAP_ARB;
    //   log_debug_multiple << "type = GL_TEXTURE_CUBE_MAP_ARB" << endl;
    // }
    //                break;
  }

  glBindTexture( dimension, m_tex_id);

  if(m_dimension != TEX_CUBE) {
    gluBuild2DMipmaps( dimension, /*format*/GL_COMPRESSED_RGB, m_images[0]->get_image_width(), m_images[0]->get_image_height(),
        format, type, m_images[0]->get_pixels());
  } else {
    if(m_images.size()<6) {
      log_error << "Trying to upload Cube Map with " << m_images.size() << " images" << endl;
      return false;
    }

    GLuint cube_map_faces[] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for( int i=0; i<6; i++ ) {
      switch(m_images[i]->get_image_type() ) {
        case Image::RGB:	   format = GL_RGB;
                             break;
        case Image::RGBA:	   format = GL_RGBA;
                             break;
        case Image::GREYSCALE: format = GL_ALPHA;
                               break;
      }
      gluBuild2DMipmaps( cube_map_faces[i], format, m_images[i]->get_image_width(), m_images[i]->get_image_height(),
          format, type, m_images[i]->get_pixels() );
    }
  }

  //		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  //Set Parameters
  if(!m_use_nearest) {
    glTexParameteri(dimension, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(dimension, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(dimension, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(dimension, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  if(!m_use_clamp) {
    glTexParameteri(dimension, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(dimension, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if(dimension == TEX_CUBE) {
      glTexParameteri(dimension, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }
  } else {
    glTexParameteri(dimension, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(dimension, GL_TEXTURE_WRAP_T, GL_CLAMP);
    if(dimension == TEX_CUBE) {
      glTexParameteri(dimension, GL_TEXTURE_WRAP_R, GL_CLAMP);
    }
  }

  // if (GL_EXT_texture_filter_anisotropic_supported){
  //   int max_anisotrophy = 1;
  //   glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotrophy);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotrophy);
  // }

  if(created) {
    log_debug_multiple << "width " << m_images[0]->get_image_width() << ", height " << m_images[0]->get_image_height() << " bind to " << m_tex_id << endl;
    log_debug_multiple << "mipmap = " << (m_use_nearest?"GL_NEAREST":"GL_LINEAR_MIPMAP_LINEAR") << endl;
    log_success_multiple << "Texture binded successful" << endl;
  } else {
    log_debug_multiple << "glGenTextures == NULL" << endl;
    log_error << "Texture uploaded unsuccessful" << endl;
  }

  return created;
}

