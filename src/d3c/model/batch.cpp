#include "batch.hpp"
#include "../misc/log.hpp"
#include "../../engine/utils.hh"

Batch::Batch(void *vertex_array, const unsigned int vertex_count
      , const unsigned int vsize, void *index_array
      , const unsigned int index_count, const unsigned int isize
      , unsigned int vertex_pos_attr, unsigned int texture_coord_attr
      , unsigned int vertex_normal_attr) {
  glGenVertexArrays(1, &m_vertexarrayobject);
  glBindVertexArray(m_vertexarrayobject);

  m_vertex_size = vsize;
  glGenBuffers(1, &m_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * m_vertex_size, vertex_array
      , GL_STATIC_DRAW);

  m_num_indices = index_count;
  m_index_size = isize;
  glGenBuffers(1, &m_indexbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * m_index_size
      , index_array, GL_STATIC_DRAW);

  glVertexAttribPointer(vertex_pos_attr,    3, GL_FLOAT, GL_FALSE, m_vertex_size, (void*)0);
  glVertexAttribPointer(texture_coord_attr, 2, GL_FLOAT, GL_FALSE, m_vertex_size, (void*)12);
  glVertexAttribPointer(vertex_normal_attr, 3, GL_FLOAT, GL_FALSE, m_vertex_size, (void*)20);
  glEnableVertexAttribArray(vertex_pos_attr);
  glEnableVertexAttribArray(texture_coord_attr);
  glEnableVertexAttribArray(vertex_normal_attr);
}

void Batch::render() {
  if (!m_vertexbuffer || !m_indexbuffer)
    die("no vbo or ebo!");

  glBindVertexArray(m_vertexarrayobject);
  glDrawElements(GL_TRIANGLES, m_num_indices, m_index_size == 2
      ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
}

