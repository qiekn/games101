#pragma once

#include <cstdint>
#include "bounds3.h"
#include "intersection.h"
#include "ray.h"
#include "utils/vector.h"

class Object {
public:
  Object() {}

  virtual ~Object() {}

  virtual bool Intersect(const Ray& ray) = 0;
  virtual bool Intersect(const Ray& ray, float&, uint32_t&) const = 0;
  virtual Intersection GetIntersection(Ray _ray) = 0;
  virtual void GetSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t&, const Vector2f&, Vector3f&,
                                    Vector2f&) const = 0;
  virtual Vector3f EvalDiffuseColor(const Vector2f&) const = 0;
  virtual Bounds3 GetBounds() = 0;
  virtual float GetArea() = 0;
  virtual void Sample(Intersection& pos, float& pdf) = 0;
  virtual bool HasEmit() = 0;
};
