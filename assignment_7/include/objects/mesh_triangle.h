#pragma once

#include <cassert>
#include <memory>
#include "bvh.h"
#include "material.h"
#include "objects/object.h"
#include "objects/triangle.h"

class MeshTriangle : public Object {
public:
  MeshTriangle(const std::string& filename, Material* mt = new Material());

  bool Intersect(const Ray& ray) override { return true; }

  bool Intersect(const Ray& ray, float& tnear, uint32_t& index) const override;

  Bounds3 GetBounds() override { return bounding_box; }

  void GetSurfaceProperties(const Vector3f& P, const Vector3f& I, const uint32_t& index, const Vector2f& uv,
                            Vector3f& N, Vector2f& st) const override;

  Vector3f EvalDiffuseColor(const Vector2f& st) const override;

  Intersection GetIntersection(Ray ray) override;

  void Sample(Intersection& pos, float& pdf) override;

  float GetArea() override { return area; }

  bool HasEmit() override { return m->HasEmission(); }

public:
  Bounds3 bounding_box;
  std::unique_ptr<Vector3f[]> vertices;
  uint32_t num_triangles;
  std::unique_ptr<uint32_t[]> vertex_index;
  std::unique_ptr<Vector2f[]> st_coordinates;
  std::vector<Triangle> triangles;
  BVHAccel* bvh;
  float area;
  Material* m;
};
