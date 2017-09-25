#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>
#include "../math/geometry.hpp"

class collision_set_bsp {
  class brush;

  struct kd_tree_node {
    kd_tree_node *child_front, *child_back;
    std::vector<const brush*> m_brushes;
    int index; // -1 = no children, 0 = yz plane, 1 = xz plane, 2 = xy plane
    float distance;

    kd_tree_node();
    ~kd_tree_node();
    void read_from_file(std::ifstream &file);
    void insert_brush(const brush *b);
    void trace(const glm::vec3 &start, glm::vec3 &end, float radius);
  };

  class brush {
    std::vector<Plane> m_planes;
    std::string m_solid;
  public:
    glm::vec3 bb_min, bb_max; // bounding box

    bool read_from_file(std::ifstream &file);
    void trace(const glm::vec3 &start, const glm::vec3 &end, float radius
        , glm::vec3 &output) const;
  };

  std::vector<glm::vec3> m_vertices;
  std::vector<brush*> m_brushes;
  kd_tree_node *m_kd_tree;
public:
  collision_set_bsp();
  ~collision_set_bsp();

  void trace(const glm::vec3 &start, glm::vec3 &end, float radius);
  void load_cm(const std::string &name);
};

