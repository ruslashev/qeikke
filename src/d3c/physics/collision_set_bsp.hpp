#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>
#include "../math/geometry.hpp"

class Collision_brush;

struct Kd_tree_node {
  Kd_tree_node *child_front, *child_back;
  std::vector<const Collision_brush*> m_brushes;
  int index; // -1 = no children, 0 = yz plane, 1 = xz plane, 2 = xy plane
  float distance;

  Kd_tree_node();
  ~Kd_tree_node();
  void read_from_file(std::ifstream &file);
  void insert_brush(const Collision_brush *brush);
  void trace(const glm::vec3 &start, glm::vec3 &end, float radius);
};

class Collision_brush {
  std::vector<Plane> m_planes;
  std::string m_solid;
public:
  glm::vec3 m_min, m_max; // bounding box

  bool read_from_file(std::ifstream &file);
  void trace(const glm::vec3 &start, const glm::vec3 &end, float radius
      , glm::vec3 &output) const;
};

class Collision_set_bsp {
  std::vector<glm::vec3> m_vertices;
  std::vector<Collision_brush*> m_brushes;
  Kd_tree_node *m_kd_tree;
public:
  Collision_set_bsp();
  ~Collision_set_bsp();

  void trace(const glm::vec3 &start, glm::vec3 &end, float radius);
  void load_cm(const std::string &name);
};

extern bool bsp_debug;
extern std::vector<const Collision_brush*> debug_bsp_collision_brushes;

