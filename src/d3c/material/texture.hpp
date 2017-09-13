#pragma once

#include <vector>
#include <string>

class Image;

class Texture {
public:
  Texture(const std::string& name);
  ~Texture();

  unsigned int get_tex_id() { return _id; }

  void set_image(Image* image);
  void set_image(unsigned int index, Image* image);

  void clear();
  void set_num_images(int count);

  bool upload_texture();
protected:
  std::vector<Image *> m_images;
  std::string m_name;

  unsigned int _id;
};

