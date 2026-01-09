#pragma once

#include <memory>
#include <vector>

#include "area_light.h"
#include "bvh.h"
#include "light.h"
#include "objects/object.h"
#include "ray.h"
#include "utils/vector.h"

class Scene {
public:
  // setting up options
  int width = 1280;
  int height = 960;
  double fov = 40;
  Vector3f background_color = Vector3f(0.235294, 0.67451, 0.843137);
  int max_depth = 1;
  float russian_roulette = 0.8;

  // creating the scene (adding objects and lights)
  std::vector<Object*> objects;
  std::vector<std::unique_ptr<Light>> lights;
  BVHAccel* bvh;

public:
  Scene(int w, int h) : width(w), height(h) {}

  void Add(Object* object) { objects.push_back(object); }

  void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

  const std::vector<Object*>& GetObjects() const { return objects; }

  const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; }

  Intersection Intersect(const Ray& ray) const;
  void BuildBVH();
  Vector3f CastRay(const Ray& ray, int depth) const;
  void SampleLight(Intersection& pos, float& pdf) const;
  bool Trace(const Ray& ray, const std::vector<Object*>& objects, float& t_near, uint32_t& index, Object** hit_object);
  std::tuple<Vector3f, Vector3f> HandleAreaLight(const AreaLight& light, const Vector3f& hit_point, const Vector3f& N,
                                                 const Vector3f& shadow_point_orig, const std::vector<Object*>& objects,
                                                 uint32_t& index, const Vector3f& dir, float specular_exponent);

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
};
