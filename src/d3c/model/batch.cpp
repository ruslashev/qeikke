#include "batch.hpp"
#include "../misc/log.hpp"

Batch::Batch() {
  m_indexbuffer=0;
  m_vertexbuffer=0;

  m_num_vertices = 0;
  m_num_indices  = 0;
  m_vertex_size = 0;
  m_index_size  = 0;

  // ironic: moving away from fixed function pipeline
  // makes for a less flexible code
  add_format( GL_VERTEX_ARRAY,   GL_FLOAT, 3,  0, 0);
  add_format( GL_TEXTURE_COORD_ARRAY, GL_FLOAT, 2, 12, 0);
  add_format( GL_NORMAL_ARRAY,   GL_FLOAT, 3, 20, 0);
  m_primitive_type = GL_TRIANGLES;
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
        glVertexPointer(format.size, type, get_vertex_size(), (char*)0 + format.offset);
        break;
      case GL_NORMAL_ARRAY:
        glNormalPointer(type, get_vertex_size(), (char*)0 + format.offset);
        break;
      case GL_TEXTURE_COORD_ARRAY:
        glClientActiveTexture(GL_TEXTURE0 + format.index);
        if(!format.index)
          glTexCoordPointer(format.size, type, get_vertex_size(), (char*)0 + format.offset);
        break;
      case GL_COLOR_ARRAY:
        glColorPointer(format.size, type, get_vertex_size(), (char*)0 + format.offset);
        break;
    }
    glEnableClientState(array);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glDrawElements(m_primitive_type, m_num_indices, get_index_size() == 2
      ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, nullptr);

  for (i = 0; i < num_formats; ++i) {
    if (get_format(i).att_type == GL_TEXTURE_COORD_ARRAY) {
      glClientActiveTexture(GL_TEXTURE0 + get_format(i).index);
    }
    glDisableClientState(get_format(i).att_type);
  }
}

