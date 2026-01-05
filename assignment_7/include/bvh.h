#pragma once

#include <vector>

#include "bounds3.h"
#include "intersection.h"
#include "object.h"
#include "ray.h"

// Forward Declarations
struct BvhNode;
struct BvhPrimitiveInfo;

// BVHAccel Declarations
inline int leaf_nodes, total_leaf_nodes, total_primitives, interior_nodes;

class BVHAccel {
public:
  enum class SplitMethod { kNaive, kSAH };

  BvhNode* root;

public:
  BVHAccel(std::vector<Object*> p, int max_prims_in_node = 1, SplitMethod split_method = SplitMethod::kNaive);
  ~BVHAccel();

  Bounds3 WorldBound() const;
  Intersection Intersect(const Ray& ray) const;
  Intersection GetIntersection(BvhNode* node, const Ray& ray) const;
  bool IntersectP(const Ray& ray) const;
  void GetSample(BvhNode* node, float p, Intersection& pos, float& pdf);
  void Sample(Intersection& pos, float& pdf);

private:
  BvhNode* RecursiveBuild(std::vector<Object*> objects);

private:
  const int max_prims_in_node_;  // primes: primitives
  const SplitMethod split_method_;
  std::vector<Object*> primitives_;
};

struct BvhNode {
public:
  BvhNode() {
    bounds = Bounds3();
    left = nullptr;
    right = nullptr;
    object = nullptr;
  }

public:
  Bounds3 bounds;
  BvhNode* left;
  BvhNode* right;
  Object* object;  // stored at leaf node
  float area;
  int split_axis = 0;
  int first_prim_offset = 0;
  int n_primitives = 0;
};
