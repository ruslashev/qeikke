#include "../engine/utils.hh"
#include "../engine/screen.hh"

screen *g_screen = new screen("qeikke", 800, 600);

void load() {
}

static void key_event(char key, bool down) {
}

static void mouse_motion_event(float xrel, float yrel, int x, int y) {
}

static void mouse_button_event(int button, bool down, int x, int y) {
}

static void update(double dt, double t) {
}

static void draw(double alpha) {
}

static void cleanup() {
}

int main() {
  g_screen->mainloop(load, key_event, mouse_motion_event, mouse_button_event
      , update, draw, cleanup);
}

