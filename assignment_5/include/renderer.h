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
  void Render(const Scene& scene);

private:
};
