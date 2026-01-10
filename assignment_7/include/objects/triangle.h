#pragma once

#include <cassert>

#include "intersection.h"
#include "material.h"
#include "objects/object.h"

// ----------------------------------------------------------------------------: helper

bool RayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& orig,
                          const Vector3f& dir, float& tnear, float& u, float& v);

// ----------------------------------------------------------------------------: class

class Triangle : public Object {
public:
  Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material* _m = nullptr);

  bool Intersect(const Ray& ray) override;
  bool Intersect(const Ray& ray, float& tnear, uint32_t& index) const override;
  Intersection GetIntersection(Ray ray) override;

  void GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                            Vector3f& N, Vector2f& st) const override;

  Vector3f EvalDiffuseColor(const Vector2f&) const override;
  Bounds3 GetBounds() override;

  void Sample(Intersection& pos, float& pdf) override;

  float GetArea() override;

  bool HasEmit() override;

public:
  Vector3f v0, v1, v2;  // vertices A, B ,C , counter-clockwise order
  Vector3f e1, e2;      // 2 edges v1-v0, v2-v0;
  Vector3f t0, t1, t2;  // texture coords
  Vector3f normal;
  float area;
  Material* m;
};
