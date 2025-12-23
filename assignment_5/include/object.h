#pragma once

#include "global.h"
#include "vector.h"

class Object {
public:
  Object()
      : material_type(kDIFFUSE_AND_GLOSSY),
        ior(1.3),
        Kd(0.8),
        Ks(0.2),
        diffuse_color(0.2),
        specular_exponent(25) {}

  virtual ~Object() = default;

  virtual bool Intersect(const Vector3f&, const Vector3f&, float&, uint32_t&, Vector2f&) const = 0;

  virtual void GetSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t&,
                                    const Vector2f&, Vector3f&, Vector2f&) const = 0;

  virtual Vector3f EvalDiffuseColor(const Vector2f&) const { return diffuse_color; }

  // material properties
  MaterialType material_type;
  float ior;
  float Kd, Ks;
  Vector3f diffuse_color;
  float specular_exponent;
};
