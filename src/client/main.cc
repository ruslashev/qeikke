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
GLfloat light_pos[] = {150.0f, 130.0f, 120.0f, 0.0f};
GLfloat light_pos2[] = {-120.0f, +120.0f, -120.0f, 0.0f};
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

  d3c_cam->get_controller()->set_position( Vector3f(100,100,50) );

  renderer->init();
  renderer->set_viewport(0, 0, 800, 600);

  std::string name = "level";

  std::string tmp = name;
  scene.load_proc(tmp.append(".proc"));

  tmp = name;
  bsp.load_cm(tmp.append(".cm"));

  renderer->upload_textures();

  static GLfloat white_light[]=  {1.0f,1.0f,1.0f,1.0f};
  static GLfloat white_light2[]=  {1.0f,0.8f,0.8f,1.0f};
  static GLfloat ambient_light[]={0.3f,0.3f,0.3f,1.0f};

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0, GL_AMBIENT,ambient_light);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,white_light2);
  glEnable(GL_LIGHT1);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
}

static void update(double dt, double t) {
}

void render_debug_lines() {
  glColor3f(1,0,0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho( 0, g_screen->get_window_width(), g_screen->get_window_height(), 0, 1, 10);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0, 0, -5);
  glDisable(GL_DEPTH_TEST);

  glBegin(GL_LINES);
  int num_lines = portal_debug_lines.size();
  for(int i=0; i<num_lines; i++) {
    glVertex2i( portal_debug_lines[i].x, g_screen->get_window_height()-portal_debug_lines[i].y );
  }
  portal_debug_lines.clear();
  glEnd();

  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

static void draw(double alpha) {
  glm::mat4 projection = glm::perspective(glm::radians(fov)
      , (float)g_screen->get_window_width() / (float)g_screen->get_window_height()
      , 0.1f, 10000.f)
    , view = cam->compute_view_mat();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light_pos2);

  Vector3f start = d3c_cam->get_position();
  d3c_cam->update(move, strafe);
  Vector3f end = d3c_cam->get_position();

  if(use_collision) {
    Vector3f end_collided = start;
    while(end_collided != end) {
      end_collided = end;
      bsp.trace( start, end, 10.0f);
    }
    d3c_cam->get_controller()->set_position(end_collided);
  }

  renderer->set_view(d3c_cam);

  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  scene.render(d3c_cam);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  if (portal_debug)
    render_debug_lines();

  glDisable(GL_DEPTH_TEST);

  if (tree_debug)
    bsp.render_tree();

  if (bsp_debug)
    bsp.render_brushes();

  glEnable(GL_DEPTH_TEST);

  glFlush();
  renderer->next_frame();
}

static void cleanup() {
  renderer->close();
}

int main() {
  g_screen->mainloop(load, key_event, mouse_motion_event, mouse_button_event
      , update, draw, cleanup);
}

