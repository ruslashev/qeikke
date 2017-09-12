//============================================================================//
// This source file is part of work done by Reinder Nijhoff (reinder@infi.nl) //
// For the latest info, see http://developer.infi.nl                          //
//                                                                            //
// You're free to use the code in any way you like, modified, unmodified or   //
// cut'n'pasted into your own work.                                           //
//                                                                            //
// Part of this source is based on work by:                                   //
//    - Humus (http://esprit.campus.luth.se/~humus/)                          //
//    - Paul Baker (http://www.paulsprojects.net)                             //
//============================================================================//

#ifndef _RENDERER_
#define _RENDERER_ 1

#include <string>
#include <map>

#include "math/vector.hpp"
#include "math/matrix.hpp"

class Batch;
class Texture;
class Camera;

//==============================================================================
//  Renderer
//==============================================================================
class Renderer {
public:
  Renderer() : m_frame(0) {}
  ~Renderer() {}

  // texture
  Texture* get_texture_from_file(const std::string & name);
  Texture* get_texture(const std::string & name);

  // unproject
  inline bool project(Vector3f vec, int &x, int &y) {
    return project( Vector4f( vec.x, vec.y, vec.z, 1.0f ), x, y);
  }
  inline bool project(Vector4f vec, int &x, int &y) {
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
  const Matrix4x4f & get_modelview_matrix()  { return m_modelview_matrix; }
  const Matrix4x4f & get_projection_matrix() { return m_projection_matrix; }

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
  Matrix4x4f m_modelview_matrix, m_projection_matrix;
  int m_viewport[4];
private:
};

extern Renderer* renderer;

#endif /* _RENDERER_ */
