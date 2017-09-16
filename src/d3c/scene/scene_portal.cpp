#include "scene_portal.hpp"
#include "../misc/log.hpp"
#include "../../engine/screen.hh"
#include "../parse_common.hh"

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

Portal_area::Portal_area(const std::string &name, int index)
  : m_name(name)
  , m_index(index)
  , m_frame_rendered((unsigned long long)-1) {
}

Portal_area::~Portal_area() {
  for (const Batch *batch : m_batches)
    delete batch;
}

const std::string& Portal_area::get_name() const {
  return m_name;
}

void Portal_area::render(const camera *cam, const glm::ivec2 &min
    , const glm::ivec2 &max) {
  if (m_frame_rendered == g_screen->get_frame_idx())
    return;
  m_frame_rendered = g_screen->get_frame_idx();
  ++portal_debug_areas_rendered;

  // set renderport, prevent visible 'rounding-error' caps
  renderer->set_renderport(min.x, min.y, max.x - min.x + 1, max.y - min.y + 1);

  for (size_t j = 0; j < m_batches.size(); ++j) {
    glBindTexture(GL_TEXTURE_2D, m_textures[j]->get_id());
    m_batches[j]->render();
  }

  for (size_t j = 0; j < m_portals.size(); ++j)
    m_portals[j]->render_from_area(cam, m_index, min, max);
}

void Portal_area::add_portal(Portal_portal *portal) {
  m_portals.push_back(portal);
}

void Portal_area::read_from_file(std::ifstream &file) {
  int num_surfaces = stream_get_next_int(file);
  for (int i = 0; i < num_surfaces; ++i) {
    std::vector<Vertex_doom3> vertices;
    std::vector<unsigned int> indices;

    std::string name = stream_get_next_string(file) + ".tga";

    texture *tex = renderer->get_texture_from_file(name.c_str());
    m_textures.push_back(tex);

    int num_verts = stream_get_next_int(file);
    int num_ind = stream_get_next_int(file);

    vertices.resize(num_verts);
    indices.resize(num_ind);

    for (int j = 0; j < num_verts; ++j) {
      vertices[j].vertex.x = stream_get_next_float(file);
      vertices[j].vertex.z = -stream_get_next_float(file);
      vertices[j].vertex.y = stream_get_next_float(file);
      vertices[j].texcoord.x = stream_get_next_float(file);
      vertices[j].texcoord.y = stream_get_next_float(file);
      vertices[j].normal.x = stream_get_next_float(file);
      vertices[j].normal.z = -stream_get_next_float(file);
      vertices[j].normal.y = stream_get_next_float(file);
    }
    for (int j = 0; j < num_ind; ++j)
      indices[j] = stream_get_next_int(file);
    Batch *batch = new Batch(vertices.data(), vertices.size()
        , sizeof(Vertex_doom3), indices.data(), indices.size()
        , sizeof(unsigned int), renderer->vertex_pos_attr
        , renderer->texture_coord_attr, renderer->vertex_normal_attr); // trashy
    m_batches.push_back(batch);
  }
}

Portal_portal::Portal_portal(Scene_portal *scene)
  : m_frame_rendered((unsigned long long)-1) {
  m_scene = scene;
}

void Portal_portal::render_from_area(const camera *cam, int index
    , glm::ivec2 min, glm::ivec2 max) {
  // check if vertices are projected for visibility check
  if (m_frame_rendered != g_screen->get_frame_idx()) {
    m_frame_rendered = g_screen->get_frame_idx();
    if (!(m_visible = check_visibility(cam))) // portal is outside frustrum
      return;
    transform_points();
  }

  if (!m_visible)
    return;
  else if (m_visible < 0) {
    // intersection of portal and front plane of frustum
    // set min and max to renderport
    m_transformed_min = glm::ivec2(0, 0);
    m_transformed_max = glm::ivec2(g_screen->get_window_width()
        , g_screen->get_window_height());
  }

  // clip min and max
  if (min.x < m_transformed_min.x)
    min.x = m_transformed_min.x;
  if (max.x > m_transformed_max.x)
    max.x = m_transformed_max.x;
  if (min.y < m_transformed_min.y)
    min.y = m_transformed_min.y;
  if (max.y > m_transformed_max.y)
    max.y = m_transformed_max.y;

  // render area if visible
  if (max.x > min.x && max.y > min.y) {
    if (index == m_area_pos)
      m_scene->get_area(m_area_neg)->render(cam, min, max);
    else
      m_scene->get_area(m_area_pos)->render(cam, min, max);
    if (portal_debug) {
      portal_add_debug_line(min, glm::ivec2(min.x, max.y));
      portal_add_debug_line(min, glm::ivec2(max.x, min.y));
      portal_add_debug_line(glm::ivec2(min.x, max.y), max);
      portal_add_debug_line(glm::ivec2(max.x, min.y), max);
    }
  }
}

void Portal_portal::read_from_file(std::ifstream &file) {
  int num_points = stream_get_next_int(file);
  int pos_area = stream_get_next_int(file);
  int neg_area = stream_get_next_int(file);

  m_points.resize(num_points);
  m_transformed_points.resize(num_points);

  for (int i = 0; i < num_points; ++i) {
    m_points[i].x = stream_get_next_float(file);
    m_points[i].z = -stream_get_next_float(file);
    m_points[i].y = stream_get_next_float(file);
  }

  m_area_pos = m_scene->get_area_index_by_name("_area"
      + std::to_string(pos_area));
  m_area_neg = m_scene->get_area_index_by_name("_area"
      + std::to_string(neg_area));

  if (m_area_pos>=0)
    m_scene->get_area(m_area_pos)->add_portal(this);
  if (m_area_neg>=0)
    m_scene->get_area(m_area_neg)->add_portal(this);
}

// 0 = invisible (outside frustrum), 1 = visible, -1 = intersects frontplane
int Portal_portal::check_visibility(const camera *cam) {
  return 1;
}

void Portal_portal::transform_points() {
  m_transformed_min = glm::ivec2( 99999, 99999);
  m_transformed_max = glm::ivec2(-99999,-99999);
  for (size_t i = 0; i < m_points.size(); ++i)
    if (renderer->project(m_points[i], m_transformed_points[i].x
          , m_transformed_points[i].y)) {
      // find maximum and minimum x, y and z values of transformed points
      // to construct renderports
      if (m_transformed_points[i].x > m_transformed_max.x)
        m_transformed_max.x = m_transformed_points[i].x;
      if (m_transformed_points[i].x < m_transformed_min.x)
        m_transformed_min.x = m_transformed_points[i].x;
      if (m_transformed_points[i].y > m_transformed_max.y)
        m_transformed_max.y = m_transformed_points[i].y;
      if (m_transformed_points[i].y < m_transformed_min.y)
        m_transformed_min.y = m_transformed_points[i].y;
    }
}

Scene_portal::~Scene_portal() {
  for (const Portal_area *area : m_areas)
    delete area;
  for (const Portal_portal *portal : m_portals)
    delete portal;
}

void Scene_portal::render(camera *cam) {
  int start_area = get_area(cam->pos);
  glm::ivec2 min = glm::ivec2(0, 0)
    , max = glm::ivec2(g_screen->get_window_width()
        , g_screen->get_window_height());

  portal_debug_areas_rendered = 0;

  if (start_area < 0)
    m_areas[-1 - start_area]->render(cam, min, max);
  else // position in the "void", render all areas
    for (Portal_area *area : m_areas)
      area->render(cam, min, max);

  renderer->set_renderport(0, 0, g_screen->get_window_width()
      , g_screen->get_window_height());
}

void Scene_portal::load_proc(const std::string &name) {
  std::ifstream file(name.c_str());
  assertf(file, "unable to open \"%s\"", name.c_str());

  std::string s;
  while (file >> s) {
    if (s == "model") { // areas
      std::string name = stream_get_next_string(file);
      Portal_area *portal_area = new Portal_area(name, m_areas.size());
      portal_area->read_from_file(file);
      m_areas.push_back(portal_area);
    } else if (s == "interAreaPortals") { // portals
      int num_areas = stream_get_next_int(file);
      int num_portals = stream_get_next_int(file);
      m_portals.resize(num_portals);
      for (int i = 0; i < num_portals; ++i) {
        Portal_portal *portal_portal = new Portal_portal(this);
        portal_portal->read_from_file(file);
        m_portals[i] = portal_portal;
      }
    } else if (s == "nodes") { // nodes
      int num_nodes = stream_get_next_int(file);
      m_nodes.resize(num_nodes);
      for (int i = 0; i < num_nodes; ++i) {
        Doom3_node node;
        node.plane.normal.x = stream_get_next_float(file);
        node.plane.normal.z = -stream_get_next_float(file);
        node.plane.normal.y = stream_get_next_float(file);
        node.plane.d = stream_get_next_float(file);
        node.pos_child = stream_get_next_int(file);
        node.neg_child = stream_get_next_int(file);
        if (node.pos_child < 0)
          node.pos_child = -1 - get_area_index_by_name("_area"
              + std::to_string(-1 - node.pos_child));
        if (node.neg_child < 0)
          node.neg_child = -1 - get_area_index_by_name("_area"
              + std::to_string(-1 - node.neg_child));
        m_nodes[i] = node;
      }
    }
  }
  file.close();
}

Portal_area* Scene_portal::get_area(int i) {
  return m_areas[i];
}

int Scene_portal::get_area(const glm::vec3 &position) {
  if (!m_nodes.size())
    return 0;

  // walk through nodes
  const Doom3_node *node = &m_nodes[0];
  while (true)
    if (node->plane.is_in_front(position)) { // in front
      if (node->pos_child > 0)
        node = &m_nodes[node->pos_child];
      else
        return node->pos_child;
    } else { // backside
      if (node->neg_child > 0)
        node = &m_nodes[node->neg_child];
      else
        return node->neg_child;
    }
}

int Scene_portal::get_area_index_by_name(const std::string &name) {
  for (size_t i = 0; i < m_areas.size(); ++i)
    if (m_areas[i]->get_name() == name)
      return i;
  return -1;
}

