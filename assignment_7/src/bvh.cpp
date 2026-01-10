#include "bvh.h"

#include <algorithm>
#include <cassert>
#include "global.h"

BVHAccel::BVHAccel(std::vector<Object*> p, int max_prims_in_node, SplitMethod split_method)
    : max_prims_in_node_(std::min(255, max_prims_in_node)), split_method_(split_method), primitives_(std::move(p)) {
  time_t start, stop;
  time(&start);
  if (primitives_.empty())
    return;

  root = RecursiveBuild(primitives_);

  time(&stop);
  double diff = difftime(stop, start);
  int hrs = (int)diff / 3600;
  int mins = ((int)diff / 60) - (hrs * 60);
  int secs = (int)diff - (hrs * 3600) - (mins * 60);

  printf("\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n", hrs, mins, secs);
}

// BVH 构建流程
BvhNode* BVHAccel::RecursiveBuild(std::vector<Object*> objects) {
  BvhNode* node = new BvhNode();

  // Compute bounds of all primitives in BVH node
  Bounds3 bounds;
  for (int i = 0; i < objects.size(); ++i) {
    bounds = Union(bounds, objects[i]->GetBounds());
  }

  // clang-format off
  // 基准情况： 只有1个物体 → 创建叶子节点
  if (objects.size() == 1) {
    node->bounds = objects[0]->GetBounds();
    node->object = objects[0];
    node->left = nullptr;
    node->right = nullptr;
    node->area = objects[0]->GetArea();
    return node;
  }
  // 两个物体 → 直接分开
  else if (objects.size() == 2) {
    node->left = RecursiveBuild(std::vector{objects[0]});
    node->right = RecursiveBuild(std::vector{objects[1]});

    node->bounds = Union(node->left->bounds, node->right->bounds);
    node->area = node->left->area + node->right->area;
    return node;
  }
  // 多个物体 → 选择最长轴分割
  else {
    Bounds3 centroid_bounds;
    for (int i = 0; i < objects.size(); ++i)
      centroid_bounds = Union(centroid_bounds, objects[i]->GetBounds().Centroid());
    int dim = centroid_bounds.MaxExtent();  // 最长轴
    // 按最长轴排序
    switch (dim) {
      case 0:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
          return f1->GetBounds().Centroid().x < f2->GetBounds().Centroid().x;
        });
        break;
      case 1:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
          return f1->GetBounds().Centroid().y < f2->GetBounds().Centroid().y;
        });
        break;
      case 2:
        std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
          return f1->GetBounds().Centroid().z < f2->GetBounds().Centroid().z;
        });
        break;
    }
    // clang-format on

    auto beginning = objects.begin();
    auto middling = objects.begin() + (objects.size() / 2);
    auto ending = objects.end();

    auto left_shapes = std::vector<Object*>(beginning, middling);
    auto right_shapes = std::vector<Object*>(middling, ending);

    assert(objects.size() == (left_shapes.size() + right_shapes.size()));

    node->left = RecursiveBuild(left_shapes);
    node->right = RecursiveBuild(right_shapes);

    node->bounds = Union(node->left->bounds, node->right->bounds);
    node->area = node->left->area + node->right->area;
  }

  return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const {
  Intersection hit;
  if (!root)
    return hit;
  hit = BVHAccel::GetIntersection(root, ray);
  return hit;
}

Intersection BVHAccel::GetIntersection(BvhNode* node, const Ray& ray) const {
  Intersection hit;

  Vector3f inv_dir = ray.direction_inv;
  std::array<bool, 3> is_dir_neg = {ray.direction.x < 0, ray.direction.y < 0, ray.direction.z < 0};

  // Check current bbox (Bounding Box)
  if (!node->bounds.IntersectP(ray, inv_dir, is_dir_neg)) {
    return hit;
  }

  // Check leaf node
  if (node->left == nullptr && node->right == nullptr) {
    return node->object->GetIntersection(ray);
  }

  // Recursively check left and right subtrees, return the closer hit
  Intersection hit_left = GetIntersection(node->left, ray);
  Intersection hit_right = GetIntersection(node->right, ray);

  return hit_left.distance < hit_right.distance ? hit_left : hit_right;
}

void BVHAccel::GetSample(BvhNode* node, float p, Intersection& pos, float& pdf) {
  if (node->left == nullptr || node->right == nullptr) {
    node->object->Sample(pos, pdf);
    pdf *= node->area;
    return;
  }
  if (p < node->left->area)
    GetSample(node->left, p, pos, pdf);
  else
    GetSample(node->right, p - node->left->area, pos, pdf);
}

void BVHAccel::Sample(Intersection& pos, float& pdf) {
  float p = std::sqrt(GetRandomFloat()) * root->area;
  GetSample(root, p, pos, pdf);
  pdf /= root->area;
}
