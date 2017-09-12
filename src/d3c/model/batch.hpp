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

#ifndef _BATCH_
#define _BATCH_ 1

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

//==============================================================================
//  Batch
//==============================================================================
class Batch {
public:
  Batch();

  const unsigned int get_vertex_count() const { return m_num_vertices;  }
  const unsigned int get_index_count()  const { return m_num_indices;   }
  const unsigned int get_vertex_size()  const { return m_vertex_size; }
  const unsigned int get_index_size()   const { return m_index_size;  }

  const unsigned int get_format_count() const { return m_formats.size(); }
  const Format &get_format(unsigned int index) const { return m_formats[index]; }

  const unsigned int get_primitive_type() const { return m_primitive_type; }
  void set_primitive_type(const unsigned int prim_type) { m_primitive_type
    = prim_type; }

  void set_vertices(void *vertex_array, const unsigned int vertex_count
      , const unsigned int size) {
    m_num_vertices = vertex_count;
    m_vertex_size = size;
    glGenBuffers(1, &m_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, m_num_vertices * m_vertex_size, vertex_array
        , GL_STATIC_DRAW);
  }
  void set_indices(void *index_array, const unsigned int index_count
      , const unsigned int size) {
    m_num_indices = index_count;
    m_index_size = size;
    glGenBuffers(1, &m_indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_num_indices * m_index_size
        , index_array, GL_STATIC_DRAW);
  }

  void add_format(const unsigned int att_type, const unsigned int att_format, const unsigned int size, const unsigned int offset, const unsigned int index = 0) {
    Format format;

    format.att_type = att_type;
    format.att_format = att_format;
    format.size = size;
    format.offset = offset;
    format.index = index;

    m_formats.push_back(format);
  }

  void render();
protected:
  unsigned int m_vertexbuffer;
  unsigned int m_indexbuffer;

  unsigned int m_num_vertices;
  unsigned int m_num_indices;
  unsigned int m_vertex_size;
  unsigned int m_index_size;

  std::vector<Format> m_formats;
  unsigned int m_primitive_type;
};

#endif /* _BATCH_ */
