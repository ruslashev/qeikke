#include "collision_set_bsp.hpp"
#include "../misc/log.hpp"
#include "../math/geometry.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <GL/gl.h>
#include <GL/glu.h>

#define EPSILON 0.001f

std::vector<Collision_brush*> debug_bsp_collision_brushes;
bool bsp_debug;

// TODO use from scene_portal
inline std::string cm_get_next_value(std::ifstream & file) {
  std::string s;
  while( file >> s) {
    if( s == "/*" ) {
      while( s != "*/") {
        file >> s;
      }
    } else if( s == "{" || s == "}") {
    } else if( s == "(" || s == ")") {
    } else {
      return s;
    }
  }
  return "EOF";
}

inline std::string cm_get_next_string(std::ifstream & file) {
  std::string s = cm_get_next_value(file);
  return s.substr(1, s.size()-2);
}

#define cm_get_next_float(x) atof( cm_get_next_value(x).c_str() )
#define cm_get_next_int(x) atoi( cm_get_next_value(x).c_str() )

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
  index = cm_get_next_int(file);
  distance = cm_get_next_float(file);

  if(index >= 0) {  // two children...
    child_front = new Kd_tree_node();
    child_back = new Kd_tree_node();
    child_front->read_from_file(file);
    child_back->read_from_file(file);
  }
}

void Kd_tree_node::insert_brush(Collision_brush* brush) {
  if(index >=0 ) { // have children, find the right child for the brush
    // very fast plane check...
    if( (brush->m_min[index] >= distance) && (brush->m_max[index] >= distance) ) {
      child_front->insert_brush( brush );
    } else if( (brush->m_min[index] <= distance) && (brush->m_max[index] <= distance) ) {
      child_back->insert_brush( brush );
    } else {
      // plane interstect brush
      // m_brushes.push_back( brush ); (insert in both children!!
      child_front->insert_brush( brush );
      child_back->insert_brush( brush );
    }
  } else {
    m_brushes.push_back( brush );
  }
}

void Kd_tree_node::trace(const glm::vec3 &start, glm::vec3 &end, float radius) {
  // walk through the k-D tree to find brushes..
  int num_brushes = m_brushes.size();

  if(num_brushes) {
    for(int i=0; i<num_brushes; ++i) {
      m_brushes[i]->trace(start, end, radius, end);
      if(bsp_debug) {
        debug_bsp_collision_brushes.push_back(m_brushes[i]);
      }
    }
  }

  if(index >=0 ) { // have children, find the right child for the brush
    // very fast plane check...
    if( (start[index] >= distance + radius) && (end[index] >= distance + radius) ) {
      child_front->trace( start, end, radius );
    } else if( (start[index] <= distance - radius) && (end[index] <= distance - radius) ) {
      child_back->trace( start, end, radius );
    } else {
      // check the node to trace first ...
      if( start[index] >= distance + radius ) {
        child_front->trace( start, end, radius );
        child_back->trace( start, end, radius );
      } else {
        child_back->trace( start, end, radius );
        child_front->trace( start, end, radius );
      }
    }
  }
}

bool Collision_brush::read_from_file(std::ifstream &file) {
  std::string tmp = cm_get_next_value(file);
  if(tmp == "EOF" || tmp == "collisionModel") {
    log_debug << "end of file" << endl;
    return false;
  }
  int num_planes = atoi( tmp.c_str() );

  m_planes.resize(num_planes);
  // planes
  Plane plane;
  for(int i=0; i<num_planes; ++i) {
    plane.normal.x = cm_get_next_float(file);
    plane.normal.z = -cm_get_next_float(file);
    plane.normal.y = cm_get_next_float(file);
    plane.d = cm_get_next_float(file);

    m_planes[i] = plane;
  }
  // bounding box
  m_min.x = cm_get_next_float(file);
  m_min.z = -cm_get_next_float(file);
  m_min.y = cm_get_next_float(file);

  m_max.x = cm_get_next_float(file);
  m_max.z = -cm_get_next_float(file);
  m_max.y = cm_get_next_float(file);

  m_solid = cm_get_next_string(file);

  return true;
}

void Collision_brush::trace(const glm::vec3 & start, const glm::vec3 & end, float radius, glm::vec3 & output) {
  float min_fraction = -99999.0f;
  int min_plane = -1;

  int num_planes = m_planes.size();

  for (int i=0; i<num_planes; ++i) {
    float start_distance, end_distance;

    start_distance = m_planes[i].distance( start ) - radius;
    end_distance   = m_planes[i].distance( end   ) - radius;

    // make sure the trace isn't completely on one side of the brush
    if (start_distance > 0 && end_distance > 0)	{
      // both are in front of the plane, its outside of this brush
      return;
    }
    if (start_distance <= 0 || end_distance >= 0) {
      // start is behind this plane
      continue;
    }

    if( end_distance > min_fraction) {
      min_fraction = end_distance;
      min_plane = i;
    }
  }

  if(min_plane >= 0) {
    if( -10.0f < min_fraction && min_fraction < 0.0f ) {
      output = end - (min_fraction-EPSILON) * m_planes[min_plane].normal;
    } else {
      log_warning << "min_fraction to large! (" << min_fraction << ")" << endl;
    }
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

void Collision_set_bsp::trace(const glm::vec3 & start, glm::vec3 & end, float radius) {
  if (m_kd_tree)
    m_kd_tree->trace(start, end, radius);
}

void Collision_set_bsp::load_cm(const std::string & name) {
  log_function << "Load " << name << " in Collision_set_bsp::load_cm()" << endl;

  std::ifstream file(name.c_str());
  std::string s;

  if(!file) {
    log_error << "unable to open " << name << endl;
    return;
  }

  while( file >> s) {
    if(s == "vertices") {
      /*		    int num_vertices = cm_get_next_int(file);

                log_debug << "load " << num_vertices << " vertices" << endl;

                m_vertices.resize(num_vertices);

                for(int i=0; i<num_vertices; ++i) {
                glm::vec3 vertex;
                vertex.x = cm_get_next_float(file);
                vertex.y = cm_get_next_float(file);
                vertex.z = cm_get_next_float(file);

                m_vertices[i] = vertex;
                }
                */
    } else if( s == "brushes") {
      log_debug << "load brushes" << endl;

      int brush_memory = cm_get_next_int(file);

      Collision_brush *brush = new Collision_brush();
      while(  brush->read_from_file(file) ) {
        m_brushes.push_back(brush);
        if(m_kd_tree) {
          m_kd_tree->insert_brush(brush);
        }
        brush = new Collision_brush();
      }
      delete brush;
      // hack, only load first collision model
      break;
    } else if( s == "nodes") {
      log_debug << "load k-D tree" << endl;

      m_kd_tree = new Kd_tree_node();
      m_kd_tree->read_from_file(file);
    }
  }
  file.close();

  log_success << name << " loaded succesfully " << endl;
}

