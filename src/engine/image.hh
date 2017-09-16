#pragma once

#include <string>

enum class image_type {
  undefined, rgb, rgba, grayscale
};

class image {
  unsigned char *_pixels;
  int _width;
  int _height;
  image_type _type;

  void _create_empty();
  bool _load_from_file(const std::string &filename);
public:
  std::string fail_reason;

  image();
  image(const std::string &filename);
  ~image();

  unsigned int get_width();
  unsigned int get_height();
  unsigned char* get_pixels();
  image_type get_image_type();
};

