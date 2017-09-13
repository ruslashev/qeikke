#pragma once

#include <string>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Batch;
class Texture;
class Camera;

class Renderer {
  int m_frame;
  std::map<std::string, Texture*> m_texture_map;

  glm::mat4 m_modelview_matrix, m_projection_matrix;
  int m_viewport[4];
public:
  Renderer();

  Texture* get_texture_from_file(const std::string & name);
  Texture* create_texture(const std::string & name);
  void upload_textures();

  // unproject
  inline bool project(glm::vec3 vec, int &x, int &y);
  inline bool project(glm::vec4 vec, int &x, int &y);

  const glm::mat4 & get_modelview_matrix()  { return m_modelview_matrix; }
  const glm::mat4 & get_projection_matrix() { return m_projection_matrix; }

  inline int get_frame() { return m_frame; }
  void next_frame() { m_frame++; }

  // init
  void init();

  // viewport
  void set_viewport(const int left, const int top, const int width
      , const int height);
  void set_renderport(const int left, const int top, const int width
      , const int height);
  void set_view(Camera* camera);

  // batch
  Batch* create_batch();
};

extern Renderer* renderer;

