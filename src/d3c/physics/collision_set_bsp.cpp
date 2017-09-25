#include "collision_set_bsp.hpp"
#include "../math/geometry.hpp"
#include "../parse_common.hh"
#include "../../engine/utils.hh"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#define EPSILON 0.001f

Kd_tree_node::Kd_tree_node()
  : child_front(nullptr)
  , child_back(nullptr) {
}

Kd_tree_node::~Kd_tree_node() {
  if (child_front)
    delete child_front;
  if (child_back)
    delete child_back;
}

void Kd_tree_node::read_from_file(std::ifstream &file) {
  index = stream_get_next_int(file);
  distance = stream_get_next_float(file);
  if (index >= 0) { // has children
    child_front = new Kd_tree_node();
    child_back = new Kd_tree_node();
    child_front->read_from_file(file);
    child_back->read_from_file(file);
  }
}

void Kd_tree_node::insert_brush(const Collision_brush *brush) {
  if (index >= 0) { // have children, find the right child for the brush
    // very fast plane check
    if (brush->m_min[index] >= distance && brush->m_max[index] >= distance)
      child_front->insert_brush(brush);
    else if (brush->m_min[index] <= distance && brush->m_max[index] <= distance)
      child_back->insert_brush(brush);
    else {
      // plane interstects brush
      // m_brushes.push_back(brush); (insert in both children!!
      child_front->insert_brush(brush);
      child_back->insert_brush(brush);
    }
  } else
    m_brushes.push_back(brush);
}

void Kd_tree_node::trace(const glm::vec3 &start, glm::vec3 &end, float radius) {
  // walk through the tree to find brushes
  for (size_t i = 0; i < m_brushes.size(); ++i)
    m_brushes[i]->trace(start, end, radius, end);

  if (index >= 0) {
    if (start[index] >= distance + radius && end[index] >= distance + radius)
      child_front->trace(start, end, radius);
    else if (start[index] <= distance - radius
        && end[index] <= distance - radius)
      child_back->trace(start, end, radius);
    else
      // check the node to trace first
      if (start[index] >= distance + radius) {
        child_front->trace(start, end, radius);
        child_back->trace(start, end, radius);
      } else {
        child_back->trace(start, end, radius);
        child_front->trace(start, end, radius);
      }
  }
}

bool Collision_brush::read_from_file(std::ifstream &file) {
  std::string val = stream_get_next_value(file);
  if (val == "EOF" || val == "collisionModel")
    return false;
  int num_planes = atoi(val.c_str());
  m_planes.resize(num_planes);
  for (int i = 0; i < num_planes; ++i) {
    m_planes[i].normal.x = stream_get_next_float(file);
    m_planes[i].normal.z = -stream_get_next_float(file);
    m_planes[i].normal.y = stream_get_next_float(file);
    m_planes[i].d = stream_get_next_float(file);
  }
  // bounding box
  m_min.x = stream_get_next_float(file);
  m_min.z = -stream_get_next_float(file);
  m_min.y = stream_get_next_float(file);
  m_max.x = stream_get_next_float(file);
  m_max.z = -stream_get_next_float(file);
  m_max.y = stream_get_next_float(file);
  m_solid = stream_get_next_string(file);

  return true;
}

void Collision_brush::trace(const glm::vec3 &start, const glm::vec3 &end
    , float radius, glm::vec3 &output) const {
  float min_fraction = -99999.0f;
  int min_plane = -1;

  for (size_t i = 0; i < m_planes.size(); ++i) {
    float start_distance = m_planes[i].distance(start) - radius
      , end_distance = m_planes[i].distance(end) - radius;

    // make sure the trace isn't completely on one side of the brush
    if (start_distance > 0 && end_distance > 0)
      // both are in front of the plane, its outside of this brush
      return;
    if (start_distance <= 0 || end_distance >= 0)
      // start is behind this plane
      continue;

    if (end_distance > min_fraction) {
      min_fraction = end_distance;
      min_plane = i;
    }
  }

  if (min_plane >= 0) {
    if (-10.f < min_fraction && min_fraction < 0.f)
      output = end - (min_fraction - EPSILON) * m_planes[min_plane].normal;
    else
      warning_ln("min_fraction too large: %f", (double)min_fraction);
  }
}

Collision_set_bsp::Collision_set_bsp()
  : m_kd_tree(nullptr) {
}

Collision_set_bsp::~Collision_set_bsp() {
  for (const Collision_brush *brush : m_brushes)
    delete brush;
  if (m_kd_tree)
    delete m_kd_tree;
}

void Collision_set_bsp::trace(const glm::vec3 &start, glm::vec3 &end
    , float radius) {
  if (m_kd_tree)
    m_kd_tree->trace(start, end, radius);
}

void Collision_set_bsp::load_cm(const std::string &name) {
  std::ifstream file(name.c_str());
  assertf(file, "unable to open \"%s\"", name.c_str());

  std::string s;
  while (file >> s) {
    if (s == "vertices") {
      /*
         int num_vertices = stream_get_next_int(file);
         m_vertices.resize(num_vertices);
         for(int i=0; i<num_vertices; ++i) {
           glm::vec3 vertex;
           vertex.x = stream_get_next_float(file);
           vertex.y = stream_get_next_float(file);
           vertex.z = stream_get_next_float(file);
           m_vertices[i] = vertex;
         }
         */
    } else if (s == "brushes") {
      int brush_memory = stream_get_next_int(file);
      Collision_brush *brush = new Collision_brush();
      while (brush->read_from_file(file)) {
        m_brushes.push_back(brush);
        if (m_kd_tree)
          m_kd_tree->insert_brush(brush);
        brush = new Collision_brush();
      }
      delete brush;
      break; // hack, only load first collision model
    } else if (s == "nodes") {
      m_kd_tree = new Kd_tree_node();
      m_kd_tree->read_from_file(file);
    }
  }

  file.close();
}

