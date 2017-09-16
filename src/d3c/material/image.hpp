#pragma once

#include <string>

enum class image_type {
  undefined, rgb, rgba, grayscale
};

class Image {
  unsigned char *_pixels;
  int _width;
  int _height;
  image_type _type;

  void _create_empty();
  bool _load_from_file(const std::string &filename);
public:
  std::string fail_reason;

  Image();
  Image(const std::string &filename);
  ~Image();

  unsigned int get_width();
  unsigned int get_height();
  unsigned char* get_pixels();
  image_type get_image_type();
};

