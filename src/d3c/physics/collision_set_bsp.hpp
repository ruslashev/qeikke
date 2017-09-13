#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/vec3.hpp>

class Plane;
class Collision_brush;

class Kd_tree_node {
public:
  Kd_tree_node() {}

  void read_from_file(std::ifstream &file);
  void insert_brush(Collision_brush* brush);

  void trace(const glm::vec3 & start, glm::vec3 & end, float radius);
public:
  Kd_tree_node* child_front;
  Kd_tree_node* child_back;
  std::vector<Collision_brush *> m_brushes;

  int index; // -1 = no childs, 0 = yz plane, 1 = xz plane, 2 = xy plane
  float distance;
};

class Collision_brush {
public:
  Collision_brush() {}

  bool read_from_file(std::ifstream &file);
  void trace(const glm::vec3 & start, const glm::vec3 & end, float radius, glm::vec3 & output);

  glm::vec3 m_min, m_max;	// bounding box
private:
  std::vector<Plane> m_planes;
  std::string m_solid;
};

class Collision_set_bsp {
public:
  Collision_set_bsp() : m_kd_tree(NULL) {}

  void trace(const glm::vec3 & start, glm::vec3 & end, float radius);

  void render_brushes();
  void render_tree();

  void load_cm(const std::string & name);
private:
  std::vector<glm::vec3> m_vertices;
  std::vector<Collision_brush *> m_brushes;
  Kd_tree_node* m_kd_tree;
};

extern bool bsp_debug;
extern std::vector<Collision_brush *> debug_bsp_collision_brushes;

