#include "objects/triangle.h"
#include "global.h"

// ----------------------------------------------------------------------------: helper

bool RayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& orig,
                          const Vector3f& dir, float& tnear, float& u, float& v) {
  Vector3f edge1 = v1 - v0;
  Vector3f edge2 = v2 - v0;
  Vector3f pvec = CrossProduct(dir, edge2);
  float det = DotProduct(edge1, pvec);
  if (det == 0 || det < 0)
    return false;

  Vector3f tvec = orig - v0;
  u = DotProduct(tvec, pvec);
  if (u < 0 || u > det)
    return false;

  Vector3f qvec = CrossProduct(tvec, edge1);
  v = DotProduct(dir, qvec);
  if (v < 0 || u + v > det)
    return false;

  float inv_det = 1 / det;

  tnear = DotProduct(edge2, qvec) * inv_det;
  u *= inv_det;
  v *= inv_det;

  return true;
}

// ----------------------------------------------------------------------------: triangle

Triangle::Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material* _m) : v0(_v0), v1(_v1), v2(_v2), m(_m) {
  e1 = v1 - v0;
  e2 = v2 - v0;
  normal = Normalize(CrossProduct(e1, e2));
  area = CrossProduct(e1, e2).Norm() * 0.5f;
}

void Triangle::GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                                    Vector3f& N, Vector2f& st) const {
  N = normal;
}

void Triangle::Sample(Intersection& pos, float& pdf) {
  float x = std::sqrt(GetRandomFloat()), y = GetRandomFloat();
  pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
  pos.normal = this->normal;
  pdf = 1.0f / area;
}

float Triangle::GetArea() {
  return area;
}

bool Triangle::HasEmit() {
  return m->HasEmission();
}

bool Triangle::Intersect(const Ray& ray) {
  return true;
}

bool Triangle::Intersect(const Ray& ray, float& tnear, uint32_t& index) const {
  return false;
}

Bounds3 Triangle::GetBounds() {
  return Union(Bounds3(v0, v1), v2);
}

Intersection Triangle::GetIntersection(Ray ray) {
  Intersection inter;

  if (DotProduct(ray.direction, normal) > 0)
    return inter;
  double u, v, t_tmp = 0;
  Vector3f pvec = CrossProduct(ray.direction, e2);
  double det = DotProduct(e1, pvec);
  if (fabs(det) < kEpsilon)
    return inter;

  double det_inv = 1. / det;
  Vector3f tvec = ray.origin - v0;
  u = DotProduct(tvec, pvec) * det_inv;
  if (u < 0 || u > 1)
    return inter;
  Vector3f qvec = CrossProduct(tvec, e1);
  v = DotProduct(ray.direction, qvec) * det_inv;
  if (v < 0 || u + v > 1)
    return inter;
  t_tmp = DotProduct(e2, qvec) * det_inv;

  if (t_tmp < 0)
    return inter;

  inter.happened = true;
  inter.coords = ray(t_tmp);
  inter.normal = normal;
  inter.distance = t_tmp;
  inter.obj = this;
  inter.m = m;

  return inter;
}

Vector3f Triangle::EvalDiffuseColor(const Vector2f&) const {
  return Vector3f(0.5, 0.5, 0.5);
}
