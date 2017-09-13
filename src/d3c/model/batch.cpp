#include "batch.hpp"
#include "../misc/log.hpp"

Batch::Batch() {
  m_indexbuffer = 0;
  m_vertexbuffer = 0;

  m_num_indices = 0;
  m_vertex_size = 0;
  m_index_size = 0;

  // ironic: moving away from fixed function pipeline
  // makes for a less flexible code
  add_format(GL_VERTEX_ARRAY,        GL_FLOAT, 3, 0, 0);
  add_format(GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 2, 12, 0);
  add_format(GL_NORMAL_ARRAY,        GL_FLOAT, 3, 20, 0);
  m_primitive_type = GL_TRIANGLES;
}

void Batch::set_vertices(void *vertex_array, const unsigned int vertex_count
    , const unsigned int size) {
  m_vertex_size = size;
  glGenBuffers(1, &m_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * m_vertex_size, vertex_array
      , GL_STATIC_DRAW);
}

void Batch::set_indices(void *index_array, const unsigned int index_count
    , const unsigned int size) {
  m_num_indices = index_count;
  m_index_size = size;
  glGenBuffers(1, &m_indexbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * m_index_size
      , index_array, GL_STATIC_DRAW);
}

void Batch::add_format(const unsigned int att_type
    , const unsigned int att_format, const unsigned int size
    , const unsigned int offset, const unsigned int index) {
  Format format;
  format.att_type = att_type;
  format.att_format = att_format;
  format.size = size;
  format.offset = offset;
  format.index = index;
  m_formats.push_back(format);
}

void Batch::render() {
  unsigned int i, num_formats = m_formats.size();

  if (!m_vertexbuffer || !m_indexbuffer)
    return;

  if (m_vertexbuffer != 0)
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

  for (i = 0; i < num_formats; ++i) {
    Format format = m_formats[i];
    GLenum type = format.att_format;
    GLenum array = format.att_type;

    switch (format.att_type) {
      case GL_VERTEX_ARRAY:
        glVertexPointer(format.size, type, m_vertex_size, (char*)0 + format.offset);
        break;
      case GL_NORMAL_ARRAY:
        glNormalPointer(type, m_vertex_size, (char*)0 + format.offset);
        break;
      case GL_TEXTURE_COORD_ARRAY:
        glClientActiveTexture(GL_TEXTURE0 + format.index);
        if(!format.index)
          glTexCoordPointer(format.size, type, m_vertex_size, (char*)0 + format.offset);
        break;
      case GL_COLOR_ARRAY:
        glColorPointer(format.size, type, m_vertex_size, (char*)0 + format.offset);
        break;
    }
    glEnableClientState(array);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glDrawElements(m_primitive_type, m_num_indices, m_index_size == 2
      ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, nullptr);

  for (i = 0; i < num_formats; ++i) {
    if (m_formats[i].att_type == GL_TEXTURE_COORD_ARRAY)
      glClientActiveTexture(GL_TEXTURE0 + m_formats[i].index);
    glDisableClientState(m_formats[i].att_type);
  }
}

