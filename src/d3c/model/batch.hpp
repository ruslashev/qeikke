#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

class Batch {
  unsigned int _vao, _vbo, _ebo;
  unsigned int _num_indices;
  unsigned int _element_size;
public:
  Batch(const void *vertex_array, unsigned int vertex_count
      , unsigned int vertex_size, const void *element_array
      , unsigned int element_count, unsigned int element_size
      , unsigned int vertex_pos_attr, unsigned int texture_coord_attr
      , unsigned int vertex_normal_attr);
  void render() const;
};

