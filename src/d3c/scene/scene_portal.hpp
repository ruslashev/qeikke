#pragma once

#include "../model/batch.hpp"
#include "../math/geometry.hpp"
#include "../renderer.hpp"
#include "../material/texture.hpp"

#include "../../engine/ogl.hh"

#include <string>
#include <vector>

class Portal_portal;
class Scene_portal;
class Camera;

struct Doom3_node {
  Plane plane;
  int pos_child, neg_child;
};

class Portal_area {
public:
  Portal_area(const std::string & name, int index) : m_name(name), m_index(index) {}
  const std::string & get_name() const { return m_name; }

  void render(Camera* camera, glm::ivec2 min, glm::ivec2 max);

  void add_portal(Portal_portal *portal) { m_portals.push_back( portal ); }

  void read_from_file(std::ifstream &file);
private:
  std::string m_name;
  int m_index;

  std::vector<Texture *> m_textures;
  std::vector<Batch *> m_batches;
  std::vector<Portal_portal *> m_portals;

  int m_frame_rendered;
};

class Portal_portal {
public:
  Portal_portal(Scene_portal* scene) : m_frame_rendered(0) { m_scene = scene; }

  void render_from_area(Camera* camera, int index, glm::ivec2 min, glm::ivec2 max);

  void read_from_file(std::ifstream &file);

  int  check_visibility(Camera* camera);
  void transform_points();
private:
  Scene_portal* m_scene;

  glm::ivec2 m_transformed_min;
  glm::ivec2 m_transformed_max;

  std::vector<glm::vec3> m_points;
  std::vector<glm::ivec2> m_transformed_points;
  int m_area_pos;
  int m_area_neg;

  int m_frame_rendered;
  int m_visible;
};

class Scene_portal {
public:
  void render(Camera* camera);
  void load_proc(const std::string & name);

  Portal_area * get_area(int i) { return m_areas[i]; }
  int get_area(const glm::vec3 & position);

  int get_area_index_by_name(const std::string & name);
private:
  std::vector<Portal_area *> m_areas;
  std::vector<Portal_portal *> m_portals;
  std::vector<Doom3_node> m_nodes;

  // shader_program _sp;
};

extern bool portal_debug;
extern int portal_debug_areas_rendered;
extern std::vector<glm::ivec2> portal_debug_lines;

void portal_add_debug_line(glm::ivec2 vec1, glm::ivec2 vec2);

