#include "../engine/utils.hh"
#include "../engine/screen.hh"
#include "../engine/camera.hh"

#include "../d3c/misc/log.hpp"
#include "../d3c/model/batch.hpp"
#include "../d3c/scene/camera.hpp"
#include "../d3c/scene/scene_portal.hpp"
#include "../d3c/physics/collision_set_bsp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

screen *g_screen = new screen("qeikke", 800, 600);

Renderer* renderer = (Renderer*)new Renderer();
Camera* d3c_cam = new Camera();
bool use_collision = true;
bool tree_debug = false;
Scene_portal scene;
Collision_set_bsp bsp;

float fov = 45.f;
camera *cam = new camera();
int move = 0, strafe = 0;
bool wireframe = false;

static void key_event(char key, bool down) {
  static bool forward = false, backward = false, left = false, right = false;
  switch (key) {
    case 'w': forward  = down; break;
    case 's': backward = down; break;
    case 'd': right    = down; break;
    case 'a': left     = down; break;
    case 'f': if (down) wireframe = !wireframe; break;
    default: break;
  }
  if (forward == backward)
    move = 0;
  else if (forward)
    move = 1;
  else if (backward)
    move = -1;
  if (right == left)
    strafe = 0;
  else if (right)
    strafe = 1;
  else if (left)
    strafe = -1;
}

static void mouse_motion_event(float xrel, float yrel, int x, int y) {
  d3c_cam->update_view_angles(xrel, yrel);
}

static void mouse_button_event(int button, bool down, int x, int y) {
}

void load() {
  g_screen->lock_mouse();

  d3c_cam->get_controller()->set_position( glm::vec3(100,100,50) );

  renderer->init();
  renderer->set_viewport(0, 0, 800, 600);

  std::string name = "level";

  scene.load_proc(name + ".proc");

  bsp.load_cm(name + ".cm");

  renderer->upload_textures();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
}

static void update(double dt, double t) {
  glm::vec3 start = d3c_cam->get_position();
  d3c_cam->update(move, strafe, dt);
  glm::vec3 end = d3c_cam->get_position();

  if(use_collision) {
    glm::vec3 end_collided = start;
    while(end_collided != end) {
      end_collided = end;
      bsp.trace( start, end, 10.0f);
    }
    d3c_cam->get_controller()->set_position(end_collided);
  }
}

static void draw(double alpha) {
  glm::mat4 projection = glm::perspective(glm::radians(fov)
      , (float)g_screen->get_window_width() / (float)g_screen->get_window_height()
      , 0.1f, 10000.f)
    , view = cam->compute_view_mat();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderer->set_view(d3c_cam);

  glEnable(GL_TEXTURE_2D);

  scene.render(d3c_cam);

  renderer->next_frame();
}

static void cleanup() {
}

int main() {
  g_screen->mainloop(load, key_event, mouse_motion_event, mouse_button_event
      , update, draw, cleanup);
}

