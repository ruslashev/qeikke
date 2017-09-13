#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

class Batch {
  unsigned int m_vertexarrayobject;
  unsigned int m_vertexbuffer;
  unsigned int m_indexbuffer;

  unsigned int m_num_vertices;
  unsigned int m_num_indices;
  unsigned int m_vertex_size;
  unsigned int m_index_size;
public:
  Batch(void *vertex_array, const unsigned int vertex_count
      , const unsigned int vsize, void *index_array
      , const unsigned int index_count, const unsigned int isize
      , unsigned int vertex_pos_attr, unsigned int texture_coord_attr
      , unsigned int vertex_normal_attr);
  void render();
};

