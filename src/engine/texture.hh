#pragma once

#include "image.hh"
#include <vector>
#include <string>

class texture {
  unsigned int _id;
  image *_image;
public:
  std::string fail_reason;

  texture(const std::string &name);
  ~texture();
  unsigned int get_id() const;
  void set_image(image* img);
  bool upload_texture();
};

