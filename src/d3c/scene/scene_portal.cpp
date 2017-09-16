#include "scene_portal.hpp"
#include "../misc/log.hpp"
#include "../../engine/screen.hh"

#include <string>
#include <sstream>
#include <iostream>

struct Vertex_doom3 {
  glm::vec3 vertex;
  glm::vec2 texcoord;
  glm::vec3 normal;
};

bool portal_debug = false;
std::vector<glm::ivec2> portal_debug_lines;
int portal_debug_areas_rendered = 0;

void portal_add_debug_line(glm::ivec2 vec1, glm::ivec2 vec2) {
  portal_debug_lines.push_back(vec1);
  portal_debug_lines.push_back(vec2);
}

void Scene_portal::render(camera* cam) {
  // find start area..
  int start_area = get_area( cam->pos );

  portal_debug_areas_rendered = 0;

  if (start_area >= 0) {
    // you're in the void!!
    int num_areas = m_areas.size();
    for(int i=0; i<num_areas; ++i) {
      m_areas[i]->render( cam, glm::ivec2(0, 0), glm::ivec2(g_screen->get_window_width(), g_screen->get_window_height()) );
    }
  } else {
    // rename start_area: -1 -> 0, -2 -> 1
    start_area = -1-start_area;
    m_areas[start_area]->render( cam, glm::ivec2(0, 0), glm::ivec2(g_screen->get_window_width(), g_screen->get_window_height()) );
  }

  renderer->set_renderport(0,0, g_screen->get_window_width(), g_screen->get_window_height());
}

void Portal_area::render(camera* cam, glm::ivec2 min, glm::ivec2 max) {
  //  hack!!?! only render area once each frame
  if(m_frame_rendered == renderer->get_frame()) {
    return;
  }
  m_frame_rendered = renderer->get_frame();
  portal_debug_areas_rendered++;

  //  set renderport, prevent visible 'rounding-error' caps
  renderer->set_renderport( min.x, min.y, max.x-min.x+1, max.y-min.y+1 );

  //  render batches in area..
  int num_batches = m_batches.size();
  for (int j = 0; j < num_batches; ++j) {
    glBindTexture(GL_TEXTURE_2D, m_textures[j]->get_id());
    m_batches[j]->render();
  }

  //  render portals
  int num_portals = m_portals.size();

  for (int j = 0; j < num_portals; ++j) {
    m_portals[j]->render_from_area( cam, m_index, min, max );
  }
}

void Portal_portal::render_from_area(camera* cam, int index, glm::ivec2 min, glm::ivec2 max) {
  // check if vertices are projected for visibility check
  if(m_frame_rendered != renderer->get_frame()) {
    m_frame_rendered = renderer->get_frame();

    if( !(m_visible = check_visibility(cam)) ) {
      // portal is outside frustrum
      return;
    }
    transform_points();
  }

  // check visibility
  if(!m_visible) {
    return;
  } else if(m_visible<0) {
    // intersection of portal and front plane of frustum
    // set min and max to renderport
    m_transformed_min = glm::ivec2(0,0);
    m_transformed_max = glm::ivec2(g_screen->get_window_width(), g_screen->get_window_height());

  }

  // clip min and max
  if(min.x < m_transformed_min.x) min.x = m_transformed_min.x;
  if(max.x > m_transformed_max.x) max.x = m_transformed_max.x;

  if(min.y < m_transformed_min.y) min.y = m_transformed_min.y;
  if(max.y > m_transformed_max.y) max.y = m_transformed_max.y;

  // render area if visible
  if( (max.x > min.x) && (max.y > min.y) ) {
    if(index == m_area_pos) {
      m_scene->get_area( m_area_neg )->render( cam, min, max );
    } else {
      m_scene->get_area( m_area_pos )->render( cam, min, max );
    }

    if(portal_debug) {
      portal_add_debug_line( min, glm::ivec2(min.x, max.y) );
      portal_add_debug_line( min, glm::ivec2(max.x, min.y) );
      portal_add_debug_line( glm::ivec2(min.x, max.y), max );
      portal_add_debug_line( glm::ivec2(max.x, min.y), max );
    }
  }
}

//==============================================================================
// 0 = invisible (outside frustrum), 1 = visible, -1 = intersects frontplane
//==============================================================================
int Portal_portal::check_visibility(camera* cam) {
  return 1;
}

void Portal_portal::transform_points() {
  int num_points = m_points.size();

  m_transformed_min = glm::ivec2( 99999, 99999);
  m_transformed_max = glm::ivec2(-99999,-99999);

  for(int i=0; i<num_points; ++i) {
    if(renderer->project( m_points[i], m_transformed_points[i].x, m_transformed_points[i].y)) {
      // find maximum and minimum x, y and z values of transformed points to construct renderports.
      if(m_transformed_points[i].x > m_transformed_max.x) {
        m_transformed_max.x = m_transformed_points[i].x;
      }
      if(m_transformed_points[i].x < m_transformed_min.x) {
        m_transformed_min.x = m_transformed_points[i].x;
      }

      if(m_transformed_points[i].y > m_transformed_max.y) {
        m_transformed_max.y = m_transformed_points[i].y;
      }
      if(m_transformed_points[i].y < m_transformed_min.y) {
        m_transformed_min.y = m_transformed_points[i].y;
      }
    }
  }
}

int Scene_portal::get_area(const glm::vec3 & position) {
  if(!m_nodes.size()) {
    return 0;
  }

  // walk through nodes...
  Doom3_node *node = &m_nodes[0];

  while(true) {
    if( node->plane.is_in_front( position ) ) { // in front
      if( node->pos_child > 0 ) {
        node = &m_nodes[ node->pos_child ];
      } else {
        return node->pos_child;
      }
    } else { // backside
      if( node->neg_child > 0 ) {
        node = &m_nodes[ node->neg_child ];
      } else {
        return node->neg_child;
      }
    }
  }
}

int Scene_portal::get_area_index_by_name(const std::string & name) {
  int num_areas = m_areas.size();

  for(int i=0; i<num_areas; ++i) {
    if(m_areas[i]->get_name() == name) {
      return i;
    }
  }
  return -1;
}

inline std::string proc_get_next_value(std::ifstream & file) {
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
  puts("HALT AND CATCH FIRE");
  exit(666);
}

inline std::string proc_get_next_string(std::ifstream & file) {
  std::string s = proc_get_next_value(file);

  return s.substr(1, s.size()-2);
}

#define proc_get_next_float(x) atof( proc_get_next_value(x).c_str() )
#define proc_get_next_int(x) atoi( proc_get_next_value(x).c_str() )

void Scene_portal::load_proc(const std::string & name) {
  std::ifstream file(name.c_str());
  assertf(file, "unable to open \"%s\"", name.c_str());

  std::string s;
  while( file >> s) {
    if(s == "model") {
      // areas
      std::string name = proc_get_next_string(file);
      Portal_area* portal_area = new Portal_area(name, m_areas.size());
      portal_area->read_from_file(file);
      m_areas.push_back(portal_area);
    } else if (s == "interAreaPortals") {
      // portals
      int num_areas   = proc_get_next_int(file);
      int num_portals = proc_get_next_int(file);

      m_portals.resize(num_portals);

      for(int i=0; i<num_portals; ++i) {
        Portal_portal* portal_portal = new Portal_portal(this);
        portal_portal->read_from_file(file);
        m_portals[i] = portal_portal;
      }
    } else if (s == "nodes") {
      // nodes
      int num_nodes = proc_get_next_int(file);

      m_nodes.resize(num_nodes);

      for(int i=0; i<num_nodes; ++i) {
        Doom3_node node;
        node.plane.normal.x = proc_get_next_float(file);
        node.plane.normal.z = -proc_get_next_float(file);
        node.plane.normal.y = proc_get_next_float(file);
        node.plane.d = proc_get_next_float(file);

        node.pos_child = proc_get_next_int(file);
        node.neg_child = proc_get_next_int(file);

        if(node.pos_child<0) {
          std::stringstream name;
          name << "_area" << (-1-node.pos_child);
          node.pos_child = -1-get_area_index_by_name(name.str());
        }

        if(node.neg_child<0) {
          std::stringstream name;
          name << "_area" << (-1-node.neg_child);
          node.neg_child = -1-get_area_index_by_name(name.str());
        }

        m_nodes[i] = node;
      }
    }
  }
  file.close();
}

void Portal_portal::read_from_file(std::ifstream &file) {
  int num_points = proc_get_next_int( file );
  int pos_area   = proc_get_next_int( file );
  int neg_area   = proc_get_next_int( file );

  m_points.resize(num_points);
  m_transformed_points.resize(num_points);

  for(int i=0; i<num_points; ++i) {
    glm::vec3 tmp;
    tmp.x = proc_get_next_float( file );
    tmp.z = -proc_get_next_float( file );
    tmp.y = proc_get_next_float( file );

    m_points[i] = tmp;
  }

  std::stringstream name_pos;
  name_pos << "_area" << pos_area;
  m_area_pos = m_scene->get_area_index_by_name(name_pos.str());
  std::stringstream name_neg;
  name_neg << "_area" << neg_area;
  m_area_neg = m_scene->get_area_index_by_name(name_neg.str());

  if(m_area_pos>=0) {
    m_scene->get_area(m_area_pos)->add_portal(this);
  }
  if(m_area_neg>=0) {
    m_scene->get_area(m_area_neg)->add_portal(this);
  }
}

void Portal_area::read_from_file(std::ifstream &file) {
  // read num surfaces;
  int num_surfaces = proc_get_next_int(file);

  for(int i=0; i<num_surfaces; i++) {
    std::vector<Vertex_doom3> vertices;
    std::vector<unsigned int> indices;

    // read surface...
    std::string name = proc_get_next_string(file) + ".tga";

    texture *tex = renderer->get_texture_from_file(name.c_str());
    m_textures.push_back(tex);

    int num_verts = proc_get_next_int(file);
    int num_ind = proc_get_next_int(file);

    vertices.resize(num_verts);
    indices.resize(num_ind);

    for (int j=0; j<num_verts; j++) {
      vertices[j].vertex.x = atof(proc_get_next_value(file).c_str());
      vertices[j].vertex.z = -atof(proc_get_next_value(file).c_str());
      vertices[j].vertex.y = atof(proc_get_next_value(file).c_str());
      vertices[j].texcoord.x = atof(proc_get_next_value(file).c_str());
      vertices[j].texcoord.y = atof(proc_get_next_value(file).c_str());
      vertices[j].normal.x = atof(proc_get_next_value(file).c_str());
      vertices[j].normal.z = -atof(proc_get_next_value(file).c_str());
      vertices[j].normal.y = atof(proc_get_next_value(file).c_str());
    }
    for (int j = 0; j < num_ind; j++)
      indices[j] = atoi(proc_get_next_value(file).c_str());
    Batch *batch = new Batch(vertices.data(), vertices.size()
        , sizeof(Vertex_doom3), indices.data(), indices.size()
        , sizeof(unsigned int), renderer->vertex_pos_attr
        , renderer->texture_coord_attr, renderer->vertex_normal_attr);
    m_batches.push_back(batch);
  }
}

