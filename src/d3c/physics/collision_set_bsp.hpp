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

#ifndef _COLLISION_SET_BSP_
#define _COLLISION_SET_BSP_ 1

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/vec3.hpp>

class Plane;
class Collision_brush;

//==============================================================================
//  Kd_tree_node
//==============================================================================
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

//==============================================================================
//  Collision_brush
//==============================================================================
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

//==============================================================================
//  Collision_set_bsp
//==============================================================================
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

//==============================================================================
//  Debug functions
//==============================================================================
extern bool bsp_debug;
extern std::vector<Collision_brush *> debug_bsp_collision_brushes;

#endif /* _COLLISION_SET_BSP_ */

