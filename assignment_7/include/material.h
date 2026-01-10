#pragma once

#include "utils/vector.h"

enum MaterialType { kDiffuse };

class Material {
public:
  Material(MaterialType t = kDiffuse, Vector3f e = Vector3f(0, 0, 0));

  MaterialType GetType();

  Vector3f GetColorAt(double u, double v);

  Vector3f GetEmission();

  bool HasEmission();

  // sample a ray by Material properties
  Vector3f Sample(const Vector3f& wi, const Vector3f& N);

  // given a ray, calculate the PdF of this ray
  float Pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);

  // given a ray, calculate the contribution of this ray
  Vector3f Eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);

private:
  // Compute reflection direction
  Vector3f Reflect(const Vector3f& I, const Vector3f& N) const;

  // Compute refraction direction using Snell's law
  //
  // We need to handle with care the two possible situations:
  // - When the ray is inside the object
  // - When the ray is outside.
  //
  // If the ray is outside, you need to make cosi positive cosi = -N.I
  // If the ray is inside, you need to invert the refractive indices and negate the normal N
  Vector3f Refract(const Vector3f& I, const Vector3f& N, const float& ior) const;

  // Compute Fresnel equation
  // I is the incident view direction
  // N is the normal at the intersection point
  // ior is the material refractive index
  // [out parm] kr is the amount of light reflected
  void Fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const;

  Vector3f ToWorld(const Vector3f& a, const Vector3f& N);

public:
  MaterialType type;
  Vector3f emission;
  float ior;
  Vector3f kd, ks;
  float specular_exponent;
};
