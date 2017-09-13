#pragma once

#include <string>

struct Image {
  Image();
  Image(const std::string &filename);
  virtual ~Image();

  enum Image_type {UNDEFINED, RGB, RGBA, GREYSCALE};

  bool load_form_file(const std::string &filename);

  void clear();
  void create_empty();

  unsigned int get_alpha_depth();
  unsigned int get_image_width();
  unsigned int get_image_height();
  unsigned int get_pixel_depth();

  unsigned char* get_pixels();
  Image_type get_image_type();
protected:
  unsigned char *m_pixels;
  unsigned int m_pixel_depth;
  unsigned int m_alpha_depth;
  unsigned int m_height;
  unsigned int m_width;
  Image_type m_type;

  bool m_loaded;
};

