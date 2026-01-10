#include "scene.h"

void Scene::BuildBVH() {
  printf(" - Generating BVH...\n\n");
  this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::kNaive);
}

Intersection Scene::Intersect(const Ray& ray) const {
  return this->bvh->Intersect(ray);
}

void Scene::SampleLight(Intersection& pos, float& pdf) const {
  float emit_area_sum = 0;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    if (objects[k]->HasEmit()) {
      emit_area_sum += objects[k]->GetArea();
    }
  }
  float p = GetRandomFloat() * emit_area_sum;
  emit_area_sum = 0;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    if (objects[k]->HasEmit()) {
      emit_area_sum += objects[k]->GetArea();
      if (p <= emit_area_sum) {
        objects[k]->Sample(pos, pdf);
        break;
      }
    }
  }
}

bool Scene::Trace(const Ray& ray, const std::vector<Object*>& objects, float& t_near, uint32_t& index,
                  Object** hit_object) {
  *hit_object = nullptr;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    float t_near_k = kInfinity;
    uint32_t index_k;
    Vector2f uv_k;
    if (objects[k]->Intersect(ray, t_near_k, index_k) && t_near_k < t_near) {
      *hit_object = objects[k];
      t_near = t_near_k;
      index = index_k;
    }
  }

  return (*hit_object != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::CastRay(const Ray& ray, int depth) const {
  // TODO: Implement Path Tracing Algorithm here
  return Vector3f();
}

void Scene::Fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const {
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

Vector3f Scene::Refract(const Vector3f& I, const Vector3f& N, const float& ior) const {
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
