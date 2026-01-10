#include "bounds3.h"

// ----------------------------------------------------------------------------: helper

Bounds3 Union(const Bounds3& b1, const Bounds3& b2) {
  Bounds3 res;  // short for result
  res.p_min = Vector3f::Min(b1.p_min, b2.p_min);
  res.p_max = Vector3f::Max(b1.p_max, b2.p_max);
  return res;
}

Bounds3 Union(const Bounds3& b, const Vector3f& p) {
  Bounds3 res;
  res.p_min = Vector3f::Min(b.p_min, p);
  res.p_max = Vector3f::Max(b.p_max, p);
  return res;
}

// ----------------------------------------------------------------------------: impl of bound3 class

bool Bounds3::IntersectP(const Ray& ray, const Vector3f& inv_dir, const std::array<bool, 3>& is_dir_neg) const {
  // is_dir_neg[i] == true: direction is negative, hit p_max first then p_min
  // is_dir_neg[i] == false: direction is positive, hit p_min first then p_max

  // Calculate the tmin and tmax for each pair
  // For example
  //   ray: r(t) = o + t * d & x = x0 (plane)
  // Solve intersection:
  //   r(t).x = ox + t * dx = x0
  //   => t = (x0 - ox) / dx

  // x axis
  double t_x_min = (p_min.x - ray.origin.x) * inv_dir.x;
  double t_x_max = (p_max.x - ray.origin.x) * inv_dir.x;
  if (is_dir_neg[0])
    std::swap(t_x_min, t_x_max);

  // y axis
  double t_y_min = (p_min.y - ray.origin.y) * inv_dir.y;
  double t_y_max = (p_max.y - ray.origin.y) * inv_dir.y;
  if (is_dir_neg[1])
    std::swap(t_y_min, t_y_max);

  // z axis
  double t_z_min = (p_min.z - ray.origin.z) * inv_dir.z;
  double t_z_max = (p_max.z - ray.origin.z) * inv_dir.z;
  if (is_dir_neg[2])
    std::swap(t_z_min, t_z_max);

  // See p38: https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_13.pdf
  double t_enter = std::max({t_x_min, t_y_min, t_z_min});
  double t_exit = std::min({t_x_max, t_y_max, t_z_max});

  // See p39 of the pdf above
  return t_enter < t_exit && t_exit >= 0;
}

Bounds3::Bounds3(const Vector3f p1, const Vector3f p2) {
  p_min = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
  p_max = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
}

Vector3f Bounds3::Diagonal() const {
  return p_max - p_min;
}

int Bounds3::MaxExtent() const {
  Vector3f d = Diagonal();
  if (d.x > d.y && d.x > d.z)
    return 0;
  else if (d.y > d.z)
    return 1;
  else
    return 2;
}

double Bounds3::SurfaceArea() const {
  Vector3f d = Diagonal();
  return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Bounds3 Bounds3::Intersect(const Bounds3& b) {
  return Bounds3(Vector3f(fmax(p_min.x, b.p_min.x), fmax(p_min.y, b.p_min.y), fmax(p_min.z, b.p_min.z)),
                 Vector3f(fmin(p_max.x, b.p_max.x), fmin(p_max.y, b.p_max.y), fmin(p_max.z, b.p_max.z)));
}

Vector3f Bounds3::Offset(const Vector3f& p) const {
  Vector3f o = p - p_min;
  if (p_max.x > p_min.x)
    o.x /= p_max.x - p_min.x;
  if (p_max.y > p_min.y)
    o.y /= p_max.y - p_min.y;
  if (p_max.z > p_min.z)
    o.z /= p_max.z - p_min.z;
  return o;
}

bool Bounds3::Overlaps(const Bounds3& b1, const Bounds3& b2) {
  bool x = (b1.p_max.x >= b2.p_min.x) && (b1.p_min.x <= b2.p_max.x);
  bool y = (b1.p_max.y >= b2.p_min.y) && (b1.p_min.y <= b2.p_max.y);
  bool z = (b1.p_max.z >= b2.p_min.z) && (b1.p_min.z <= b2.p_max.z);
  return (x && y && z);
}

bool Bounds3::Inside(const Vector3f& p, const Bounds3& b) {
  // clang-format off
    return (p.x >= b.p_min.x && p.x <= b.p_max.x &&
            p.y >= b.p_min.y && p.y <= b.p_max.y &&
            p.z >= b.p_min.z && p.z <= b.p_max.z);
  // clang-format on
}
