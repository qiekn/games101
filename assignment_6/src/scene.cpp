//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "scene.h"

void Scene::BuildBVH() {
  printf(" - Generating BVH...\n\n");
  this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::kNaive);
}

Intersection Scene::Intersect(const Ray& ray) const {
  return this->bvh->Intersect(ray);
}

bool Scene::Trace(const Ray& ray, const std::vector<Object*>& objects, float& tNear, uint32_t& index,
                  Object** hitObject) {
  *hitObject = nullptr;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    float tNearK = kInfinity;
    uint32_t indexK;
    Vector2f uvK;
    if (objects[k]->Intersect(ray, tNearK, indexK) && tNearK < tNear) {
      *hitObject = objects[k];
      tNear = tNearK;
      index = indexK;
    }
  }

  return (*hitObject != nullptr);
}

// Implementation of the Whitted-syle light transport algorithm (E [S*] (D|G) L)
//
// This function is the function that compute the color at the intersection point
// of a ray defined by a position and a direction. Note that thus function is recursive (it calls
// itself).
//
// If the material of the intersected object is either reflective or reflective and refractive,
// then we compute the reflection/refracton direction and cast two new rays into the scene
// by calling the castRay() function recursively. When the surface is transparent, we mix
// the reflection and refraction color using the result of the fresnel equations (it computes
// the amount of reflection and refractin depending on the surface normal, incident view direction
// and surface refractive index).
//
// If the surface is duffuse/glossy we use the Phong illumation model to compute the color
// at the intersection point.
Vector3f Scene::CastRay(const Ray& ray, int depth) const {
  if (depth > this->max_depth) {
    return Vector3f(0.0, 0.0, 0.0);
  }
  Intersection intersection = Scene::Intersect(ray);
  Material* m = intersection.m;
  Object* hitObject = intersection.obj;
  Vector3f hitColor = this->background_color;
  //    float tnear = kInfinity;
  Vector2f uv;
  uint32_t index = 0;
  if (intersection.happened) {
    Vector3f hitPoint = intersection.coords;
    Vector3f N = intersection.normal;  // normal
    Vector2f st;                       // st coordinates
    hitObject->GetSurfaceProperties(hitPoint, ray.direction, index, uv, N, st);
    //        Vector3f tmp = hitPoint;
    switch (m->GetType()) {
      case REFLECTION_AND_REFRACTION: {
        Vector3f reflectionDirection = Normalize(Reflect(ray.direction, N));
        Vector3f refractionDirection = Normalize(Refract(ray.direction, N, m->ior));
        Vector3f reflectionRayOrig =
            (DotProduct(reflectionDirection, N) < 0) ? hitPoint - N * kEpsilon : hitPoint + N * kEpsilon;
        Vector3f refractionRayOrig =
            (DotProduct(refractionDirection, N) < 0) ? hitPoint - N * kEpsilon : hitPoint + N * kEpsilon;
        Vector3f reflectionColor = CastRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1);
        Vector3f refractionColor = CastRay(Ray(refractionRayOrig, refractionDirection), depth + 1);
        float kr;
        Fresnel(ray.direction, N, m->ior, kr);
        hitColor = reflectionColor * kr + refractionColor * (1 - kr);
        break;
      }
      case REFLECTION: {
        float kr;
        Fresnel(ray.direction, N, m->ior, kr);
        Vector3f reflectionDirection = Reflect(ray.direction, N);
        Vector3f reflectionRayOrig =
            (DotProduct(reflectionDirection, N) < 0) ? hitPoint + N * kEpsilon : hitPoint - N * kEpsilon;
        hitColor = CastRay(Ray(reflectionRayOrig, reflectionDirection), depth + 1) * kr;
        break;
      }
      default: {
        // [comment]
        // We use the Phong illumation model int the default case. The phong model
        // is composed of a diffuse and a specular reflection component.
        // [/comment]
        Vector3f lightAmt = 0, specularColor = 0;
        Vector3f shadowPointOrig =
            (DotProduct(ray.direction, N) < 0) ? hitPoint + N * kEpsilon : hitPoint - N * kEpsilon;
        // [comment]
        // Loop over all lights in the scene and sum their contribution up
        // We also apply the lambert cosine law
        // [/comment]
        for (uint32_t i = 0; i < GetLights().size(); ++i) {
          auto area_ptr = dynamic_cast<AreaLight*>(this->GetLights()[i].get());
          if (area_ptr) {
            // Do nothing for this assignment
          } else {
            Vector3f lightDir = GetLights()[i]->position - hitPoint;
            // square of the distance between hitPoint and the light
            float lightDistance2 = DotProduct(lightDir, lightDir);
            lightDir = Normalize(lightDir);
            float LdotN = std::max(0.f, DotProduct(lightDir, N));
            Object* shadowHitObject = nullptr;
            float tNearShadow = kInfinity;
            // is the point in shadow, and is the nearest occluding object closer to the object than
            // the light itself?
            bool inShadow = bvh->Intersect(Ray(shadowPointOrig, lightDir)).happened;
            lightAmt += (1 - inShadow) * GetLights()[i]->intensity * LdotN;
            Vector3f reflectionDirection = Reflect(-lightDir, N);
            specularColor +=
                powf(std::max(0.f, -DotProduct(reflectionDirection, ray.direction)), m->specular_exponent) *
                GetLights()[i]->intensity;
          }
        }
        hitColor = lightAmt * (hitObject->EvalDiffuseColor(st) * m->kd + specularColor * m->ks);
        break;
      }
    }
  }

  return hitColor;
}
