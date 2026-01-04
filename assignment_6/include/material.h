//
// Created by LEI XU on 5/16/19.
//

#pragma once

#include "vector.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };

class Material {
public:
  Material(MaterialType t = DIFFUSE_AND_GLOSSY, Vector3f c = Vector3f(1, 1, 1), Vector3f e = Vector3f(0, 0, 0))
      : type_(t), color_(c), emission_(e) {}

  MaterialType GetType() { return type_; }

  Vector3f GetColor() { return color_; }

  Vector3f GetEmission() { return emission_; }

  Vector3f GetColorAt(double u, double v) { return Vector3f(); }

public:
  float ior;
  float kd, ks;
  float specular_exponent;

private:
  MaterialType type_;
  Vector3f color_;
  Vector3f emission_;
};
