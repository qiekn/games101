//
// Created by LEI XU on 5/16/19.
//

#pragma once

#include "vector.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Material {
public:
  MaterialType type_;
  Vector3f color_;
  Vector3f emission_;
  float ior_;
  float kd_, ks_;
  float specular_exponent_;
  // Texture tex;

  inline Material(MaterialType t = DIFFUSE_AND_GLOSSY, Vector3f c = Vector3f(1, 1, 1), Vector3f e = Vector3f(0, 0, 0));
  inline MaterialType GetType();
  inline Vector3f GetColor();
  inline Vector3f GetColorAt(double u, double v);
  inline Vector3f GetEmission();
};

Material::Material(MaterialType t, Vector3f c, Vector3f e) {
  type_ = t;
  color_ = c;
  emission_ = e;
}

MaterialType Material::GetType() {
  return type_;
}

Vector3f Material::GetColor() {
  return color_;
}

Vector3f Material::GetEmission() {
  return emission_;
}

Vector3f Material::GetColorAt(double u, double v) {
  return Vector3f();
}
