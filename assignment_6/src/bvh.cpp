#include "bvh.h"
#include <algorithm>
#include <cassert>

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod)
    : max_prims_in_node_(std::min(255, maxPrimsInNode)), split_method_(splitMethod), primitives(std::move(p)) {
  time_t start, stop;
  time(&start);
  if (primitives.empty())
    return;

  root = RecursiveBuild(primitives);

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
    return node;
  } 
  // 两个物体 → 直接分开
  else if (objects.size() == 2) {
    node->left = RecursiveBuild(std::vector{objects[0]});
    node->right = RecursiveBuild(std::vector{objects[1]});

    node->bounds = Union(node->left->bounds, node->right->bounds);
    return node;
  }
  // 多个物体 → 选择最长轴分割
  else {
    Bounds3 centroidBounds;
    for (int i = 0; i < objects.size(); ++i)
      centroidBounds = Union(centroidBounds, objects[i]->GetBounds().Centroid());
    int dim = centroidBounds.MaxExtent(); // 最长轴
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
  }

  return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const {
  Intersection isect;
  if (!root)
    return isect;
  isect = BVHAccel::GetIntersection(root, ray);
  return isect;
}

Intersection BVHAccel::GetIntersection(BvhNode* node, const Ray& ray) const {
  // TODO Traverse the BVH to find intersection

  // 1. 如果光线不与当前节点的包围盒相交，返回空
  // 2. 如果是叶子节点（有 object），返回与该物体的交点
  // 3. 否则，递归检查左右子树，返回更近的交点
}
