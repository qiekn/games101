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
  // - When the ray is inside the object
  // - When the ray is outside.
  //
  // If the ray is outside, you need to make cosi positive cosi = -N.I
  // If the ray is inside, you need to invert the refractive indices and negate the normal N
  Vector3f Refract(const Vector3f& I, const Vector3f& N, const float& ior) const;

  // Compute Fresnel equation
  //
  // I is the incident view direction
  // N is the normal at the intersection point
  // ior is the material refractive index
  // kr is the amount of light reflected
  void Fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const;

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
};
