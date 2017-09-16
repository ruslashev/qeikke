#include "image.hh"
#include <fstream>
#include "utils.hh"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wduplicated-branches"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include "../thirdparty/stb_image.h"
#pragma GCC diagnostic pop

void image::_create_empty() {
  if (_pixels)
    free(_pixels);
  _width = 256;
  _height = 256;
  _type = image_type::rgb;
  unsigned int pixel_depth = 24, pd_byte = pixel_depth / 8;

  // not using `new' for same deallocation api with stb_image (free())
  _pixels = (unsigned char*)malloc(_width * _height * pd_byte);
  for (int i = 0; i < 3; ++i)
    for (int y = 0; y < _height; ++y)
      for (int x = 0; x < _width; ++x)
        _pixels[y * _width * pd_byte + x * pd_byte + i] = x ^ y;
}

bool image::_load_from_file(const std::string &filename) {
  std::string real_filename;
  std::ifstream existence_check_fs(filename, std::ios::binary);
  if (!existence_check_fs) {
    // this check exists because levels that came with doom3_collision have
    // texture paths "../data/textures/*". but since files were moved
    // around, they should actually be "data/textures/*".
    std::string transformed_filename = filename;
    transformed_filename.erase(transformed_filename.begin()
        , transformed_filename.begin() + 3);
    existence_check_fs.open(transformed_filename, std::ios::binary);
    if (!existence_check_fs) {
      fail_reason = "failed to open file \"" + filename + "\" or \""
        + transformed_filename + "\"";
      return false;
    } else
      real_filename = transformed_filename;
  } else
    real_filename = filename;

  int channels;
  _pixels = stbi_load(real_filename.c_str(), &_width, &_height, &channels, 0);
  if (!_pixels) {
    fail_reason = std::string(stbi_failure_reason());
    return false;
  }

  switch (channels) {
    case 1: _type = image_type::grayscale; break;
    case 3: _type = image_type::rgb; break;
    case 4: _type = image_type::rgba; break;
    default:
      fail_reason = "unsupported # of channels: " + std::to_string(channels);
      return false;
  }

  return true;
}

image::image(const std::string &filename) {
  _pixels = nullptr;
  _width = _height = 0;
  _type = image_type::undefined;
  fail_reason = "";
  if (!_load_from_file(filename)) {
    warning_ln("failed to create image \"%s\": %s", filename.c_str()
        , fail_reason.c_str());
    _create_empty();
  }
}

image::~image() {
  if (_pixels)
    free(_pixels);
}

unsigned int image::get_width() {
  return _width;
}

unsigned int image::get_height() {
  return _height;
}

unsigned char* image::get_pixels() {
  return _pixels;
}

image_type image::get_image_type() {
  return _type;
}

