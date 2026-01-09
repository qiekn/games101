#pragma once

#include <cmath>

#include "global.h"
#include "utils/vector.h"

enum MaterialType { kDiffuse };

class Material {
public:
  inline Material(MaterialType t = kDiffuse, Vector3f e = Vector3f(0, 0, 0));
  inline MaterialType GetType();
  inline Vector3f GetColorAt(double u, double v);
  inline Vector3f GetEmission();
  inline bool HasEmission();

  // sample a ray by Material properties
  inline Vector3f Sample(const Vector3f& wi, const Vector3f& N);
  // given a ray, calculate the PdF of this ray
  inline float Pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
  // given a ray, calculate the contribution of this ray
  inline Vector3f Eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);

private:
  // Compute reflection direction
  Vector3f Reflect(const Vector3f& I, const Vector3f& N) const { return I - 2 * DotProduct(I, N) * N; }

  // Compute refraction direction using Snell's law
  //
  // We need to handle with care the two possible situations:
  //
  //    - When the ray is inside the object
  //
  //    - When the ray is outside.
  //
  // If the ray is outside, you need to make cosi positive cosi = -N.I
  //
  // If the ray is inside, you need to invert the refractive indices and negate the normal N
  Vector3f Refract(const Vector3f& I, const Vector3f& N, const float& ior) const {
    float cosi = Clamp(-1, 1, DotProduct(I, N));
    float etai = 1, etat = ior;
    Vector3f n = N;
    if (cosi < 0) {
      cosi = -cosi;
    } else {
      std::swap(etai, etat);
      n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
  }

  // Compute Fresnel equation
  //
  // \param I is the incident view direction
  //
  // \param N is the normal at the intersection point
  //
  // \param ior is the material refractive index
  //
  // \param[out] kr is the amount of light reflected
  void Fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const {
    float cosi = Clamp(-1, 1, DotProduct(I, N));
    float etai = 1, etat = ior;
    if (cosi > 0) {
      std::swap(etai, etat);
    }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
      kr = 1;
    } else {
      float cost = sqrtf(std::max(0.f, 1 - sint * sint));
      cosi = fabsf(cosi);
      float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
      float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
      kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
  }

  Vector3f ToWorld(const Vector3f& a, const Vector3f& N) {
    Vector3f B, C;
    if (std::fabs(N.x) > std::fabs(N.y)) {
      float inv_len = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
      C = Vector3f(N.z * inv_len, 0.0f, -N.x * inv_len);
    } else {
      float inv_len = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
      C = Vector3f(0.0f, N.z * inv_len, -N.y * inv_len);
    }
    B = CrossProduct(C, N);
    return a.x * B + a.y * C + a.z * N;
  }

public:
  MaterialType type;
  Vector3f emission;
  float ior;
  Vector3f kd, ks;
  float specular_exponent;
};

Material::Material(MaterialType t, Vector3f e) {
  type = t;
  emission = e;
}

MaterialType Material::GetType() {
  return type;
}

Vector3f Material::GetEmission() {
  return emission;
}

bool Material::HasEmission() {
  if (emission.Norm() > kEpsilon)
    return true;
  else
    return false;
}

Vector3f Material::GetColorAt(double u, double v) {
  return Vector3f();
}

Vector3f Material::Sample(const Vector3f& wi, const Vector3f& N) {
  switch (type) {
    case kDiffuse: {
      // uniform sample on the hemisphere
      float x_1 = GetRandomFloat(), x_2 = GetRandomFloat();
      float z = std::fabs(1.0f - 2.0f * x_1);
      float r = std::sqrt(1.0f - z * z), phi = 2 * kPi * x_2;
      Vector3f local_ray(r * std::cos(phi), r * std::sin(phi), z);
      return ToWorld(local_ray, N);

      break;
    }
  }
  return Vector3f();
}

float Material::Pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
  switch (type) {
    case kDiffuse: {
      // uniform sample probability 1 / (2 * PI)
      if (DotProduct(wo, N) > 0.0f)
        return 0.5f / kPi;
      else
        return 0.0f;
      break;
    }
  }
  return 0.0f;
}

Vector3f Material::Eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
  switch (type) {
    case kDiffuse: {
      // calculate the contribution of diffuse model
      float cos_alpha = DotProduct(N, wo);
      if (cos_alpha > 0.0f) {
        Vector3f diffuse = kd / kPi;
        return diffuse;
      } else {
        return Vector3f(0.0f);
      }
      break;
    }
  }
  return Vector3f();
}
