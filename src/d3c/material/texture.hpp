#pragma once

#include "../../engine/image.hh"
#include <vector>
#include <string>

class Texture {
public:
  Texture(const std::string& name);
  ~Texture();

  unsigned int get_tex_id() { return _id; }

  void set_image(image* img);
  void set_image(unsigned int index, image* img);

  void clear();
  void set_num_images(int count);

  bool upload_texture();
protected:
  std::vector<image*> m_images;
  std::string m_name;

  unsigned int _id;
};

