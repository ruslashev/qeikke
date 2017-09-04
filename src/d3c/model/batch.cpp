//============================================================================//
// This source file is part of work done by Reinder Nijhoff (reinder@infi.nl) //
// For the latest info, see http://developer.infi.nl                          //
//                                                                            //
// You're free to use the code in any way you like, modified, unmodified or   //
// cut'n'pasted into your own work.                                           //
//                                                                            //
// Part of this source is based on work by:                                   //
//    - Humus (http://esprit.campus.luth.se/~humus/)                          //
//    - Paul Baker (http://www.paulsprojects.net)                             //
//============================================================================//

#include "batch.hpp"
#include "../math/vector.hpp"
#include "../misc/log.hpp"

//==============================================================================
//  Batch::Batch()
//==============================================================================
Batch::Batch() {
  m_indexbuffer=0;
  m_vertexbuffer=0;

  m_vertices = NULL;
  m_indices  = NULL;

  m_num_vertices = 0;
  m_num_indices  = 0;
  m_vertex_size = 0;
  m_index_size  = 0;
}

//==============================================================================
//  Batch::~Batch()
//==============================================================================
Batch::~Batch() {
  clean();
}

//==============================================================================
//  Batch::clean()
//==============================================================================
void Batch::clean() {
  if (m_vertices != NULL) delete m_vertices;
  if (m_indices  != NULL) delete m_indices;
  m_vertices = NULL;
  m_indices = NULL;
}

void Batch::upload_vertexbuffer() {
  glGenBuffers(1, &m_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, m_num_vertices * m_vertex_size, m_vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &m_indexbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * m_index_size, m_indices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//==============================================================================
//  Batch_opengl::render()
//==============================================================================
void Batch::render() {
  unsigned int i, num_formats = m_formats.size();

  if(!m_vertexbuffer){
    upload_vertexbuffer();
  }

  if (m_vertexbuffer != 0) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
  }

  for (i = 0; i < num_formats; ++i) {
    Format format = m_formats[i];
    GLenum type = format.att_format;
    GLenum array = format.att_type;

    char *basePointer = (m_vertexbuffer)? NULL : get_vertices();

    switch (format.att_type) {
      case GL_VERTEX_ARRAY:
        glVertexPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
      case GL_NORMAL_ARRAY:
        glNormalPointer(type, get_vertex_size(), basePointer + format.offset);
        break;
      case GL_TEXTURE_COORD_ARRAY:
        glClientActiveTexture(GL_TEXTURE0 + format.index);
        if(!format.index)
          glTexCoordPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
      case GL_COLOR_ARRAY:
        glColorPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
    }
    glEnableClientState(array);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
  glDrawElements(get_primitive_type(), get_index_count(), get_index_size() == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  for (i = 0; i < num_formats; ++i) {
    if (get_format(i).att_type == GL_TEXTURE_COORD_ARRAY) {
      glClientActiveTexture(GL_TEXTURE0 + get_format(i).index);
    }
    glDisableClientState(get_format(i).att_type);
  }

  if (m_vertexbuffer) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

