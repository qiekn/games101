#pragma once

#include "vector.h"

struct Ray {
public:
  // Destination = origin + t * direction
  Vector3f origin;
  Vector3f direction,  direction_inv;
  double t;  // transportation time
  double t_min, t_max;

  Ray(const Vector3f& ori, const Vector3f& dir, const double t = 0.0) : origin(ori), direction(dir), t(t) {
    direction_inv = Vector3f(1. / dir.x, 1. / dir.y, 1. / dir.z);
    t_min = 0.0;
    t_max = std::numeric_limits<double>::max();
  }

  Vector3f operator()(double t) const { return origin + direction * t; }

  friend std::ostream& operator<<(std::ostream& os, const Ray& r) {
    os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
    return os;
  }
};
