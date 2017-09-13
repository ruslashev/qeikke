#include "utils.hh"

namespace shaders {

static const char *map_vert = _glsl(
  attribute vec3 vertex_pos;
  attribute vec2 texture_coord;
  attribute vec3 vertex_normal;
  uniform mat4 mvp;
  varying vec2 texture_coord_f;
  varying vec3 vertex_normal_f;
  void main() {
    gl_Position = mvp * vec4(vertex_pos, 1.0);
    texture_coord_f = texture_coord;
    vertex_normal_f = vertex_normal;
  }
);

static const char *map_frag = _glsl(
  varying vec2 texture_coord_f;
  varying vec3 vertex_normal_f;
  uniform sampler2D texture_sampler;
  void main() {
    gl_FragColor = vec4(texture2D(texture_sampler, texture_coord_f).xyz, 1.0);
  }
);

static const char *simple_vert = _glsl(
  attribute vec3 vertex_pos;
  uniform mat4 mvp;
  void main() {
    gl_Position = mvp * vec4(vertex_pos, 1.0);
  }
);

static const char *simple_frag = _glsl(
  void main() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
);

static const char *colored_mesh_vert = _glsl(
  attribute vec3 vertex_pos;
  attribute vec3 vertex_color;
  varying vec3 vertex_color_f;
  uniform mat4 mvp;
  void main() {
    gl_Position = mvp * vec4(vertex_pos, 1.0);
    vertex_color_f = vertex_color;
  }
);

static const char *colored_mesh_frag = _glsl(
  varying vec3 vertex_color_f;
  void main() {
    gl_FragColor = vec4(vertex_color_f, 1.0);
  }
);

};

