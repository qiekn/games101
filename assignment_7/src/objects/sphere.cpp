#include "objects/sphere.h"
#include "ray.h"

bool Sphere::Intersect(const Ray& ray) {
  // analytic solution
  Vector3f L = ray.origin - center;
  float a = DotProduct(ray.direction, ray.direction);
  float b = 2 * DotProduct(ray.direction, L);
  float c = DotProduct(L, L) - radius2;
  float t0, t1;
  float area = 4 * kPi * radius2;
  if (!SolveQuadratic(a, b, c, t0, t1))
    return false;
  if (t0 < 0)
    t0 = t1;
  if (t0 < 0)
    return false;
  return true;
}

bool Sphere::Intersect(const Ray& ray, float& tnear, uint32_t& index) const {
  // analytic solution
  Vector3f L = ray.origin - center;
  float a = DotProduct(ray.direction, ray.direction);
  float b = 2 * DotProduct(ray.direction, L);
  float c = DotProduct(L, L) - radius2;
  float t0, t1;
  if (!SolveQuadratic(a, b, c, t0, t1))
    return false;
  if (t0 < 0)
    t0 = t1;
  if (t0 < 0)
    return false;
  tnear = t0;

  return true;
}

Intersection Sphere::GetIntersection(Ray ray) {
  Intersection result;
  result.happened = false;
  Vector3f L = ray.origin - center;
  float a = DotProduct(ray.direction, ray.direction);
  float b = 2 * DotProduct(ray.direction, L);
  float c = DotProduct(L, L) - radius2;
  float t0, t1;
  if (!SolveQuadratic(a, b, c, t0, t1))
    return result;
  if (t0 < 0)
    t0 = t1;
  if (t0 < 0)
    return result;
  result.happened = true;

  result.coords = Vector3f(ray.origin + ray.direction * t0);
  result.normal = Normalize(Vector3f(result.coords - center));
  result.m = this->m;
  result.obj = this;
  result.distance = t0;
  return result;
}

void Sphere::GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                                  Vector3f& N, Vector2f& st) const {
  N = Normalize(P - center);
}

Vector3f Sphere::EvalDiffuseColor(const Vector2f& st) const {
  return Vector3f();
}

Bounds3 Sphere::GetBounds() {
  return Bounds3(Vector3f(center.x - radius, center.y - radius, center.z - radius),
                 Vector3f(center.x + radius, center.y + radius, center.z + radius));
}

void Sphere::Sample(Intersection& pos, float& pdf) {
  float theta = 2.0 * kPi * GetRandomFloat(), phi = kPi * GetRandomFloat();
  Vector3f dir(std::cos(phi), std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta));
  pos.coords = center + radius * dir;
  pos.normal = dir;
  pos.emit = m->GetEmission();
  pdf = 1.0f / area;
}
