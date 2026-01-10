#pragma once

#include "bounds3.h"
#include "global.h"
#include "material.h"
#include "objects/object.h"
#include "utils/vector.h"

class Sphere : public Object {
public:
  Sphere(const Vector3f& c, const float& r, Material* mt = new Material())
      : center(c), radius(r), radius2(r * r), m(mt), area(4 * kPi * r * r) {}

  bool Intersect(const Ray& ray) override;

  bool Intersect(const Ray& ray, float& tnear, uint32_t& index) const override;

  Intersection GetIntersection(Ray ray) override;

  void GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                            Vector3f& N, Vector2f& st) const override;

  Vector3f EvalDiffuseColor(const Vector2f& st) const override;

  Bounds3 GetBounds() override;

  void Sample(Intersection& pos, float& pdf) override;

  float GetArea() override { return area; }

  bool HasEmit() override { return m->HasEmission(); }

public:
  Vector3f center;
  float radius, radius2;
  Material* m;
  float area;
};
