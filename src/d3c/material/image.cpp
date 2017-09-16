#include "image.hpp"
#include "../misc/log.hpp"
#include <fstream>
#include <cstring>
#include "../../engine/utils.hh"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "../../thirdparty/stb_image.h"

void Image::_create_empty() {
  if (_pixels)
    free(_pixels);
  _width = 64;
  _height = 64;
  _type = image_type::rgb;
  unsigned int pixel_depth = 24, pixel_depth_byte = pixel_depth / 8;

  // not using `new' for same deallocation api with stb_image (free())
  _pixels = (unsigned char*)malloc(_width * _height * pixel_depth_byte);
  memset(_pixels, 255, _width * _height * pixel_depth_byte);
  memset(_pixels, 0, _width * pixel_depth_byte); // top line
  for (unsigned int i = 0; i < _height; ++i) // left line
    _pixels[i * _width * pixel_depth_byte]
      = _pixels[i * _width * pixel_depth_byte + 1]
      = _pixels[i * _width * pixel_depth_byte + 2] = 0;
}

bool Image::_load_from_file(const std::string &filename) {
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

Image::Image(const std::string &filename) {
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

Image::~Image() {
  if (_pixels)
    free(_pixels);
}

unsigned int Image::get_width() {
  return _width;
}

unsigned int Image::get_height() {
  return _height;
}

unsigned char* Image::get_pixels() {
  return _pixels;
}

image_type Image::get_image_type() {
  return _type;
}

