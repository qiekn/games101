#pragma once

#include "scene.h"

struct HitPayload {
  float t_near;
  uint32_t index;
  Vector2f uv;
  Object* hit_obj;
};

class Renderer {
public:
  // The main render function.
  // This where we iterate over all pixels in the image, generate primary rays and cast these
  // rays into the scene. The content of the framebuffer is saved to a file.
  void Render(const Scene& scene);
};
