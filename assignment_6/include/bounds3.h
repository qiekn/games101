//
// Created by LEI XU on 5/16/19.
// Modified by qiekn on 12/16/2025
//

#pragma once

#include <array>
#include <limits>
#include "ray.h"
#include "vector.h"

class Bounds3 {
public:
  Vector3f p_min, p_max;  // two points to specify the bounding box

public:
  Bounds3() {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    p_max = Vector3f(min, min, min);
    p_min = Vector3f(max, max, max);
  }

  Bounds3(const Vector3f p) : p_min(p), p_max(p) {}

  Bounds3(const Vector3f p1, const Vector3f p2) {
    p_min = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
    p_max = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
  }

  Vector3f Diagonal() const { return p_max - p_min; }

  // Returns the index of the axis with the maximum extent (0=x, 1=y, 2=z).
  int MaxExtent() const {
    Vector3f d = Diagonal();
    if (d.x > d.y && d.x > d.z)
      return 0;
    else if (d.y > d.z)
      return 1;
    else
      return 2;
  }

  double SurfaceArea() const {
    Vector3f d = Diagonal();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
  }

  Vector3f Centroid() { return 0.5 * p_min + 0.5 * p_max; }

  // Returns the intersection of this bounding box and another bounding box.
  // The resulting box represents the overlapping region of the two bounds.
  // If the bounds do not overlap, the returned box may be invalid (p_min > p_max).
  Bounds3 Intersect(const Bounds3& b) {
    return Bounds3(Vector3f(fmax(p_min.x, b.p_min.x), fmax(p_min.y, b.p_min.y), fmax(p_min.z, b.p_min.z)),
                   Vector3f(fmin(p_max.x, b.p_max.x), fmin(p_max.y, b.p_max.y), fmin(p_max.z, b.p_max.z)));
  }

  // Returns the relative offset of point p within the bounding box.
  // The result is a normalized coordinate in [0, 1]^3, where (0,0,0)
  // corresponds to p_min and (1,1,1) corresponds to p_max.
  Vector3f Offset(const Vector3f& p) const {
    Vector3f o = p - p_min;
    if (p_max.x > p_min.x)
      o.x /= p_max.x - p_min.x;
    if (p_max.y > p_min.y)
      o.y /= p_max.y - p_min.y;
    if (p_max.z > p_min.z)
      o.z /= p_max.z - p_min.z;
    return o;
  }

  bool Overlaps(const Bounds3& b1, const Bounds3& b2) {
    bool x = (b1.p_max.x >= b2.p_min.x) && (b1.p_min.x <= b2.p_max.x);
    bool y = (b1.p_max.y >= b2.p_min.y) && (b1.p_min.y <= b2.p_max.y);
    bool z = (b1.p_max.z >= b2.p_min.z) && (b1.p_min.z <= b2.p_max.z);
    return (x && y && z);
  }

  bool Inside(const Vector3f& p, const Bounds3& b) {
    // clang-format off
    return (p.x >= b.p_min.x && p.x <= b.p_max.x &&
            p.y >= b.p_min.y && p.y <= b.p_max.y &&
            p.z >= b.p_min.z && p.z <= b.p_max.z);
    // clang-format on
  }

  // Allows array-style access to the bounding box corners.
  // index 0 returns p_min, index 1 returns p_max.
  inline const Vector3f& operator[](int i) const { return (i == 0) ? p_min : p_max; }

  inline bool IntersectP(const Ray& ray, const Vector3f& inv_dir, const std::array<int, 3>& dir_is_neg) const;
};

inline bool Bounds3::IntersectP(const Ray& ray, const Vector3f& inv_dir, const std::array<int, 3>& dir_is_neg) const {
  // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
  // dir_is_neg: ray direction(x,y,z), dir_is_neg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
  // TODO test if ray bound intersects
}

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2) {
  Bounds3 res;  // short for result
  res.p_min = Vector3f::Min(b1.p_min, b2.p_min);
  res.p_max = Vector3f::Max(b1.p_max, b2.p_max);
  return res;
}

inline Bounds3 Union(const Bounds3& b, const Vector3f& p) {
  Bounds3 res;
  res.p_min = Vector3f::Min(b.p_min, p);
  res.p_max = Vector3f::Max(b.p_max, p);
  return res;
}
