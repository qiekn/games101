#pragma once

#include "utils/vector.h"

class Light {
public:
  Light(const Vector3f& p, const Vector3f& i) : position(p), intensity(i) {}

  virtual ~Light() = default;

public:
  Vector3f position;
  Vector3f intensity;
};
