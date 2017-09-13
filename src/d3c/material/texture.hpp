#pragma once

#include <vector>
#include <string>

class Image;

class Texture {
public:
  Texture(const std::string& name);
  ~Texture();

  enum Type { BYTE, FLOAT };
  enum Dimension { TEX_1D, TEX_2D, TEX_3D, TEX_CUBE };

  void set_use_nearest( bool n_use_nearest) { m_use_nearest = n_use_nearest; }
  void set_use_clamp( bool n_use_clamp) { m_use_clamp = n_use_clamp; }

  unsigned int get_tex_id() { return m_tex_id; }

  void set_image(Image* image);
  void set_image(int index, Image* image);

  void clear();
  void set_num_images(int count);

  bool upload_texture();
protected:
  std::vector<Image *> m_images;
  std::string m_name;

  Type m_type;
  Dimension m_dimension;

  bool m_use_nearest;
  bool m_use_clamp;

  unsigned int m_tex_id;
};

