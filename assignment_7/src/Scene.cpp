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
