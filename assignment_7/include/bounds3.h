#pragma once

#include <array>
#include <limits>

#include "ray.h"
#include "utils/vector.h"

// ----------------------------------------------------------------------------: class

class Bounds3 {
public:
  Bounds3() {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    p_max = Vector3f(min, min, min);
    p_min = Vector3f(max, max, max);
  }

  Bounds3(const Vector3f p) : p_min(p), p_max(p) {}

  Bounds3(const Vector3f p1, const Vector3f p2);

  Vector3f Diagonal() const;

  // Returns the index of the axis with the maximum extent (0=x, 1=y, 2=z).
  int MaxExtent() const;

  double SurfaceArea() const;

  Vector3f Centroid() { return 0.5 * p_min + 0.5 * p_max; }

  // Returns the intersection of this bounding box and another bounding box.
  // The resulting box represents the overlapping region of the two bounds.
  // If the bounds do not overlap, the returned box may be invalid (p_min > p_max).
  Bounds3 Intersect(const Bounds3& b);

  // Returns the relative offset of point p within the bounding box.
  // The result is a normalized coordinate in [0, 1]^3, where (0,0,0)
  // corresponds to p_min and (1,1,1) corresponds to p_max.
  Vector3f Offset(const Vector3f& p) const;

  bool Overlaps(const Bounds3& b1, const Bounds3& b2);

  bool Inside(const Vector3f& p, const Bounds3& b);

  // Allows array-style access to the bounding box corners.
  // index 0 returns p_min, index 1 returns p_max.
  const Vector3f& operator[](int i) const { return (i == 0) ? p_min : p_max; }

  bool IntersectP(const Ray& ray, const Vector3f& inv_dir, const std::array<bool, 3>& is_dir_neg) const;

public:
  Vector3f p_min, p_max;  // two points to specify the bounding box
};

// ----------------------------------------------------------------------------: helper

Bounds3 Union(const Bounds3& b1, const Bounds3& b2);

Bounds3 Union(const Bounds3& b, const Vector3f& p);
