#pragma once

#include "../model/batch.hpp"
#include "../math/geometry.hpp"
#include "../renderer.hpp"
#include "../../engine/texture.hh"
#include "../../engine/ogl.hh"
#include "../../engine/camera.hh"

#include <string>
#include <vector>

class Portal_portal;
class Scene_portal;

struct Doom3_node {
  Plane plane;
  int pos_child, neg_child;
};

class Portal_area {
  std::string m_name;
  int m_index;

  std::vector<texture*> m_textures;
  std::vector<Batch*> m_batches;
  std::vector<Portal_portal*> m_portals;

  unsigned long long m_frame_rendered;
public:
  Portal_area(const std::string &name, int index);
  ~Portal_area();
  const std::string &get_name() const;
  void render(const camera *cam, const glm::ivec2 &min, const glm::ivec2 &max);
  void add_portal(Portal_portal *portal);
  void read_from_file(std::ifstream &file);
};

class Portal_portal {
  Scene_portal* m_scene;

  glm::ivec2 m_transformed_min;
  glm::ivec2 m_transformed_max;

  std::vector<glm::vec3> m_points;
  std::vector<glm::ivec2> m_transformed_points;
  int m_area_pos;
  int m_area_neg;

  unsigned long long m_frame_rendered;
  int m_visible;
public:
  Portal_portal(Scene_portal *scene);
  void render_from_area(const camera *cam, int index, glm::ivec2 min
      , glm::ivec2 max);
  void read_from_file(std::ifstream &file);
  int check_visibility(const camera *cam);
  void transform_points();
};

class Scene_portal {
  std::vector<Portal_area*> m_areas;
  std::vector<Portal_portal*> m_portals;
  std::vector<Doom3_node> m_nodes;
public:
  ~Scene_portal();
  void render(camera *cam);
  void load_proc(const std::string &name);
  Portal_area* get_area(int i);
  int get_area(const glm::vec3 &position);
  int get_area_index_by_name(const std::string & name);
};

extern bool portal_debug;
extern int portal_debug_areas_rendered;
extern std::vector<glm::ivec2> portal_debug_lines;

void portal_add_debug_line(glm::ivec2 vec1, glm::ivec2 vec2);

