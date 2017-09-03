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

#include <GL/glew.h>

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

bool Batch::upload_vertexbuffer() {
  glGenBuffersARB(1, &m_vertexbuffer);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertexbuffer);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_vertices * m_vertex_size, m_vertices, GL_STATIC_DRAW_ARB);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  if (m_indices > 0) {
    glGenBuffersARB(1, &m_indexbuffer);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexbuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_num_indices * m_index_size, m_indices, GL_STATIC_DRAW_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }

  return true;
}

GLenum getGLType(const Attribute_format attFormat) {
  switch (attFormat){
    case ATT_FLOAT:         return GL_FLOAT;
    case ATT_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
    default: return 0;
  }
}

GLenum getGLPrimitive(const Primitive_type primType){
  switch (primType){
    case PRIM_TRIANGLES:      return GL_TRIANGLES;
    case PRIM_QUADS:          return GL_QUADS;
    case PRIM_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    default: return 0;
  }
}

GLenum arrayType[] = { GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY };

//==============================================================================
//  Batch_opengl::render()
//==============================================================================
void Batch::render() {
  unsigned int i, num_formats = m_formats.size();

  if(!m_vertexbuffer){
    upload_vertexbuffer();
  }

  if (m_vertexbuffer != 0) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertexbuffer);
  }

  for (i = 0; i < num_formats; ++i) {
    Format format = m_formats[i];
    GLenum type = getGLType(format.att_format);
    GLenum array = arrayType[format.att_type];

    char *basePointer = (m_vertexbuffer)? NULL : get_vertices();

    switch (format.att_type) {
      case ATT_VERTEX:
        glVertexPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
      case ATT_NORMAL:
        glNormalPointer(type, get_vertex_size(), basePointer + format.offset);
        break;
      case ATT_TEXCOORD:
        glClientActiveTextureARB(GL_TEXTURE0_ARB + format.index);
        if(!format.index)
          glTexCoordPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
      case ATT_COLOR:
        glColorPointer(format.size, type, get_vertex_size(), basePointer + format.offset);
        break;
    }
    glEnableClientState(array);
  }

  if (m_indexbuffer) {
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexbuffer);
    glDrawElements(getGLPrimitive( get_primitive_type() ), get_index_count(), get_index_size() == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, NULL);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  } else {
    glDrawElements(getGLPrimitive( get_primitive_type() ), get_index_count(), get_index_size() == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, get_indices());
  }

  for (i = 0; i < num_formats; ++i) {
    if (get_format(i).att_type == ATT_TEXCOORD) {
      glClientActiveTextureARB(GL_TEXTURE0_ARB + get_format(i).index);
    }
    glDisableClientState(arrayType[get_format(i).att_type]);
  }

  if (m_vertexbuffer) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

