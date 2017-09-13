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
public:
  Renderer() : m_frame(0) {}
  ~Renderer() {}

  // texture
  Texture* get_texture_from_file(const std::string & name);
  Texture* get_texture(const std::string & name);

  // unproject
  inline bool project(glm::vec3 vec, int &x, int &y) {
    return project( glm::vec4( vec.x, vec.y, vec.z, 1.0f ), x, y);
  }
  inline bool project(glm::vec4 vec, int &x, int &y) {
    vec = m_projection_matrix * (m_modelview_matrix * vec);

    if( vec.w == 0) {
      x = y = 0;
      return false;
    }

    vec.x /= vec.w;
    vec.y /= vec.w;

    vec.x *= 0.5f;
    vec.y *= 0.5f;

    x = (int) ((vec.x + 0.5f) * (float)m_viewport[2]);
    y = (int) ((vec.y + 0.5f) * (float)m_viewport[3]);

    return true;
  }
  const glm::mat4 & get_modelview_matrix()  { return m_modelview_matrix; }
  const glm::mat4 & get_projection_matrix() { return m_projection_matrix; }

  // frames..
  inline int get_frame() { return m_frame; }
  void next_frame() { m_frame++; }

  // Renderer_opengl
  // init
  void init();

  // viewport
  void set_viewport(const int left, const int top, const int width, const int height);
  void set_renderport(const int left, const int top, const int width, const int height);
  void set_view(Camera* camera);

  // batch
  Batch* create_batch();

  // texture
  Texture* create_texture(const std::string & name);
  void upload_textures();
  void bind_texture(Texture* texture);
protected:
  int m_frame;
  std::map<std::string, Texture*> m_texture_map;

  // use voor unproject..
  glm::mat4 m_modelview_matrix, m_projection_matrix;
  int m_viewport[4];
private:
};

extern Renderer* renderer;

