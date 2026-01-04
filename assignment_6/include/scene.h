/**
 * @file scene.h
 * @brief Scene class definition for ray tracing
 * @author Göksu Güvendiren
 * @date 2019-05-14
 */

#pragma once

#include <memory>
#include "area_light.h"
#include "bvh.h"

class Scene {
public:
  Scene(int w, int h) : width(w), height(h) {}

  void Add(Object* object) { objects.push_back(object); }

  void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

  const std::vector<Object*>& GetObjects() const { return objects; }

  const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; }

  Intersection Intersect(const Ray& ray) const;

  void BuildBVH();

  // Cast a ray into the scene and compute color
  Vector3f CastRay(const Ray& ray, int depth) const;

  // Trace a ray throught the scene
  // Return true if intersection found
  bool Trace(const Ray& ray, const std::vector<Object*>& objects, float& t_near, uint32_t& index, Object** hit_object);

  std::tuple<Vector3f, Vector3f> HandleAreaLight(const AreaLight& light, const Vector3f& hit_point, const Vector3f& N,
                                                 const Vector3f& shadowPointOrig, const std::vector<Object*>& objects,
                                                 uint32_t& index, const Vector3f& dir, float specularExponent);

  Vector3f Reflect(const Vector3f& I, const Vector3f& N) const { return I - 2 * DotProduct(I, N) * N; }

  /**
   * @brief Compute refraction direction using Snell's law
   *
   * Handles two possible situations:
   * - When the ray is inside the object
   * - When the ray is outside the object
   *
   * If the ray is outside, cosi needs to be positive: cosi = -N.I
   * If the ray is inside, refractive indices are inverted and normal N is negated
   *
   * @param I Incident ray direction
   * @param N Surface normal
   * @param ior Index of refraction
   * @return Refracted ray direction (returns 0 if total internal reflection occurs)
   */
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

  /**
   * @brief Compute Fresnel equation for reflectance
   *
   * Calculates the amount of light reflected at the interface between
   * two media with different refractive indices.
   *
   * @param I Incident view direction
   * @param N Normal at the intersection point
   * @param ior Material refractive index
   * @param[out] kr Amount of light reflected (0-1)
   *
   * @note Transmittance kt = 1 - kr (by energy conservation)
   */
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

public:
  int width = 1280;
  int height = 960;
  double fov = 90;
  Vector3f background_color = Vector3f(0.235294, 0.67451, 0.843137);
  int max_depth = 5;  //  Maximum ray tracing recursion depth

  BVHAccel* bvh;  // BVH acceleration structure for the scene

  std::vector<Object*> objects;
  std::vector<std::unique_ptr<Light>> lights;
};
