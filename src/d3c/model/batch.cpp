#include "batch.hpp"
#include "../misc/log.hpp"
#include "../../engine/utils.hh"

Batch::Batch(const void *vertex_array, unsigned int vertex_count
      , unsigned int vertex_size, const void *element_array
      , unsigned int element_count, unsigned int element_size
      , unsigned int vertex_pos_attr, unsigned int texture_coord_attr
      , unsigned int vertex_normal_attr) {
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * vertex_size, vertex_array
      , GL_STATIC_DRAW);

  _num_indices = element_count;
  _element_size = element_size;
  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _num_indices * _element_size
      , element_array, GL_STATIC_DRAW);

  glVertexAttribPointer(vertex_pos_attr,    3, GL_FLOAT, GL_FALSE, vertex_size
      , (void*)0);
  glVertexAttribPointer(texture_coord_attr, 2, GL_FLOAT, GL_FALSE, vertex_size
      , (void*)12);
  glVertexAttribPointer(vertex_normal_attr, 3, GL_FLOAT, GL_FALSE, vertex_size
      , (void*)20);
  glEnableVertexAttribArray(vertex_pos_attr);
  glEnableVertexAttribArray(texture_coord_attr);
  glEnableVertexAttribArray(vertex_normal_attr);
}

void Batch::render() const {
  if (!_vbo || !_ebo)
    warning_ln("no vbo or ebo!");

  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _num_indices, _element_size == 2
      ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
}

