#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

struct Format {
  unsigned int att_type;
  unsigned int att_format;
  unsigned int size;
  unsigned int offset;
  unsigned int index;
};

class Batch {
  unsigned int m_vertexbuffer;
  unsigned int m_indexbuffer;

  unsigned int m_num_vertices;
  unsigned int m_num_indices;
  unsigned int m_vertex_size;
  unsigned int m_index_size;

  std::vector<Format> m_formats;
  unsigned int m_primitive_type;
public:
  Batch();
  void set_vertices(void *vertex_array, const unsigned int vertex_count
      , const unsigned int size);
  void set_indices(void *index_array, const unsigned int index_count
      , const unsigned int size);
  void add_format(const unsigned int att_type, const unsigned int att_format
      , const unsigned int size, const unsigned int offset
      , const unsigned int index = 0);
  void render();
};

