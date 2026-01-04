//
// Created by LEI XU on 5/16/19.
//

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
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;

class BVHAccel {
public:
  enum class SplitMethod { kNaive, kSAH };

  BVHAccel(std::vector<Object*> p, int max_prims_in_node = 1, SplitMethod split_method = SplitMethod::kNaive);

  Bounds3 WorldBound() const;

  ~BVHAccel();

  Intersection Intersect(const Ray& ray) const;

  Intersection GetIntersection(BvhNode* node, const Ray& ray) const;

  bool IntersectP(const Ray& ray) const;

  BvhNode* root;

private:
  BvhNode* RecursiveBuild(std::vector<Object*> objects);

private:
  const int max_prims_in_node_;  // primes: primitives
  const SplitMethod split_method_;
  std::vector<Object*> primitives;
};

struct BvhNode {
  BvhNode() {
    bounds = Bounds3();
    left = nullptr;
    right = nullptr;
    object = nullptr;
  }

  int split_axis = 0, first_prim_offset = 0, n_primitives = 0;

  Bounds3 bounds;
  BvhNode* left;
  BvhNode* right;
  Object* object;  // stored at leaf node
};
