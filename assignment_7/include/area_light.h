#pragma once

#include "global.h"
#include "light.h"
#include "utils/vector.h"

class AreaLight : public Light {
public:
  AreaLight(const Vector3f& p, const Vector3f& i) : Light(p, i) {
    normal = Vector3f(0, -1, 0);
    u = Vector3f(1, 0, 0);
    v = Vector3f(0, 0, 1);
    length = 100;
  }

  Vector3f SamplePoint() const {
    auto random_u = GetRandomFloat();
    auto random_v = GetRandomFloat();
    return position + random_u * u + random_v * v;
  }

public:
  float length;
  Vector3f normal;
  Vector3f u;
  Vector3f v;
};
