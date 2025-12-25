/**
 * @file scene.h
 * @brief Scene class definition for ray tracing
 * @author Göksu Güvendiren
 * @date 2019-05-14
 */

#pragma once

#include <memory>
#include <vector>
#include "area_light.h"
#include "bvh.h"
#include "light.h"
#include "object.h"
#include "ray.h"
#include "vector.h"

/**
 * @class Scene
 * @brief Main scene class containing objects, lights and rendering parameters
 */
class Scene {
public:
  int width = 1280;
  int height = 960;
  double fov = 90;
  Vector3f background_color_ = Vector3f(0.235294, 0.67451, 0.843137);
  int max_depth_ = 5;  // Maximum ray tracing recursion depth

  Scene(int w, int h) : width(w), height(h) {}

  void Add(Object* object) { objects.push_back(object); }

  void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

  const std::vector<Object*>& GetObjects() const { return objects; }

  const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; }

  /**
   * @brief Find intersection of ray with scene objects
   */
  Intersection Intersect(const Ray& ray) const;

  // BVH acceleration structure for the scene
  BVHAccel* bvh;

  void BuildBVH();

  /**
   * @brief Cast a ray into the scene and compute color
   * @param ray The ray to cast
   * @param depth Current recursion depth
   * @return Computed color at the ray intersection
   */
  Vector3f CastRay(const Ray& ray, int depth) const;

  /**
   * @brief Trace a ray through the scene
   * @param ray The ray to trace
   * @param objects Objects to test for intersection
   * @param tNear Output parameter for nearest intersection distance
   * @param index Output parameter for object index
   * @param hitObject Output parameter for pointer to hit object
   * @return True if intersection found, false otherwise
   */
  bool Trace(const Ray& ray, const std::vector<Object*>& objects, float& tNear, uint32_t& index, Object** hitObject);

  /**
   * @brief Handle lighting computation for area lights
   * @param light The area light
   * @param hitPoint The surface point being lit
   * @param N Surface normal at hit point
   * @param shadowPointOrig Origin point for shadow rays
   * @param objects Objects in the scene for shadow testing
   * @param index Object index
   * @param dir View direction
   * @param specularExponent Material specular exponent
   * @return Tuple of diffuse and specular color contributions
   */
  std::tuple<Vector3f, Vector3f> HandleAreaLight(const AreaLight& light, const Vector3f& hitPoint, const Vector3f& N,
                                                 const Vector3f& shadowPointOrig, const std::vector<Object*>& objects,
                                                 uint32_t& index, const Vector3f& dir, float specularExponent);

  std::vector<Object*> objects;
  std::vector<std::unique_ptr<Light>> lights;

  /**
   * @brief Compute reflection direction
   * @param I Incident ray direction
   * @param N Surface normal
   * @return Reflected ray direction
   */
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
};
