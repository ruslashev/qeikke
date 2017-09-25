#pragma once

#include "../model/batch.hpp"
#include "../math/geometry.hpp"
#include "../renderer.hpp"
#include "../../engine/texture.hh"
#include "../../engine/ogl.hh"
#include "../../engine/camera.hh"

#include <string>
#include <vector>

class Scene_portal {
  // share same index so batches can reference their textures
  // TODO texture id should be moved to batch class
  std::vector<const texture*> _textures;
  std::vector<Batch*> _batches;
  int _last_texture;

  void _read_model(std::ifstream &file);
public:
  Scene_portal();
  ~Scene_portal();
  void render(const camera *cam);
  void load_proc(const std::string &name);
};

