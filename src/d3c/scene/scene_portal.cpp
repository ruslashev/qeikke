#include "scene_portal.hpp"
#include "../../engine/screen.hh"
#include "../parse_common.hh"

#include <string>
#include <iostream>

struct Vertex_doom3 {
  glm::vec3 vertex;
  glm::vec2 texcoord;
  glm::vec3 normal;
};

Scene_portal::Scene_portal()
  : _last_texture(-1) {
}

Scene_portal::~Scene_portal() {
  for (const Batch *const b : _batches)
    delete b;
}

void Scene_portal::render(const camera *cam) {
  for (size_t i = 0; i < _batches.size(); ++i) {
    if (_last_texture != (int)_textures[i]->get_id()) {
      _last_texture = _textures[i]->get_id();
      glBindTexture(GL_TEXTURE_2D, _textures[i]->get_id());
    }
    _batches[i]->render();
  }
}

void Scene_portal::_read_model(std::ifstream &file) {
  int num_surfaces = stream_get_next_int(file);
  for (int i = 0; i < num_surfaces; ++i) {
    std::string texture_name = stream_get_next_string(file) + ".tga";
    texture *tex = renderer->get_texture_from_file(texture_name.c_str());
    _textures.push_back(tex);

    std::vector<Vertex_doom3> surf_vertices;
    std::vector<unsigned int> surf_elements;
    int num_vertices = stream_get_next_int(file)
      , num_elements = stream_get_next_int(file);

    surf_vertices.resize(num_vertices);
    surf_elements.resize(num_elements);

    for (int j = 0; j < num_vertices; ++j) {
      surf_vertices[j].vertex.x = stream_get_next_float(file);
      surf_vertices[j].vertex.z = -stream_get_next_float(file);
      surf_vertices[j].vertex.y = stream_get_next_float(file);
      surf_vertices[j].texcoord.x = stream_get_next_float(file);
      surf_vertices[j].texcoord.y = stream_get_next_float(file);
      surf_vertices[j].normal.x = stream_get_next_float(file);
      surf_vertices[j].normal.z = -stream_get_next_float(file);
      surf_vertices[j].normal.y = stream_get_next_float(file);
    }
    for (int j = 0; j < num_elements; ++j)
      surf_elements[j] = stream_get_next_int(file);

    Batch *b = new Batch(surf_vertices.data(), surf_vertices.size()
        , sizeof(Vertex_doom3), surf_elements.data(), surf_elements.size()
        , sizeof(unsigned int), renderer->vertex_pos_attr
        , renderer->texture_coord_attr, renderer->vertex_normal_attr); // trashy
    _batches.push_back(b);
  }
}

void Scene_portal::load_proc(const std::string &name) {
  std::ifstream file(name.c_str());
  assertf(file, "unable to open \"%s\"", name.c_str());

  std::string s;
  while (file >> s) {
    if (s == "model") {
      stream_get_next_string(file);
      _read_model(file);
    } else if (s == "interAreaPortals") {
      int num_areas = stream_get_next_int(file);
      int num_portals = stream_get_next_int(file);
      for (int i = 0; i < num_portals; ++i) {
        int num_points = stream_get_next_int(file);
        int pos_area = stream_get_next_int(file);
        int neg_area = stream_get_next_int(file);
        for (int i = 0; i < num_points; ++i) {
          stream_get_next_float(file);
          stream_get_next_float(file);
          stream_get_next_float(file);
        }
      }
    } else if (s == "nodes") {
      int num_nodes = stream_get_next_int(file);
      for (int i = 0; i < num_nodes; ++i) {
        stream_get_next_float(file);
        stream_get_next_float(file);
        stream_get_next_float(file);
        stream_get_next_float(file);
        stream_get_next_int(file);
        stream_get_next_int(file);
      }
    }
  }
  file.close();
}

