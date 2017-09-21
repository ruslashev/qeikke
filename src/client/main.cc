#include "../engine/utils.hh"
#include "../engine/screen.hh"
#include "../engine/camera.hh"

#include "../d3c/model/batch.hpp"
#include "../d3c/scene/scene_portal.hpp"
#include "../d3c/physics/collision_set_bsp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

screen *g_screen = new screen("qeikke", 800, 600);

Renderer *renderer = new Renderer();
Scene_portal scene;
Collision_set_bsp bsp;

camera *cam = new camera();
int move = 0, strafe = 0;
bool wireframe = false, collision = true;

static void key_event(char key, bool down) {
  static bool forward = false, backward = false, left = false, right = false;
  switch (key) {
    case 'w': forward  = down; break;
    case 's': backward = down; break;
    case 'd': right    = down; break;
    case 'a': left     = down; break;
    case 'f': if (down) wireframe = !wireframe; break;
    case 'c': if (down) collision = !collision; break;
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
  cam->update_view_angles(xrel, yrel);
}

static void mouse_button_event(int button, bool down, int x, int y) {
}

static void window_resize_event(int width, int height) {
  renderer->set_viewport(0, 0, width, height);
}

static void load() {
  g_screen->lock_mouse();

  cam->pos = glm::vec3(100, 50, -100);

  std::string name = "data/maps/level/level";

  scene.load_proc(name + ".proc");

  bsp.load_cm(name + ".cm");

  renderer->upload_textures();
}

static void update(double dt, double t) {
  glm::vec3 start = cam->pos;
  cam->update_position(dt, move, strafe);
  glm::vec3 end = cam->pos;

  if (collision) {
    glm::vec3 end_collided = start;
    while (end_collided != end) {
      end_collided = end;
      bsp.trace(start, end, 10.0f);
    }
    cam->pos = end_collided;
  }
}

static void draw(double alpha) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  renderer->set_view(cam);

  scene.render(cam);
}

static void cleanup() {
  delete cam;
  delete renderer;
  delete g_screen;
}

int main() {
  g_screen->mainloop(load, key_event, mouse_motion_event, mouse_button_event
      , window_resize_event, update, draw, cleanup);
}

