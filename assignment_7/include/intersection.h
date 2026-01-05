#pragma once

#include <limits>

#include "vector.h"

class Material;
class Object;
class Sphere;

struct Intersection {
public:
  bool happened;
  Vector3f coords;
  Vector3f tcoords;
  Vector3f normal;
  Vector3f emit;
  double distance;
  Object* obj;
  Material* m;

public:
  Intersection() {
    happened = false;
    coords = Vector3f();
    normal = Vector3f();
    distance = std::numeric_limits<double>::max();
    obj = nullptr;
    m = nullptr;
  }
};
