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

BVHBuildNode* BVHAccel::RecursiveBuild(std::vector<Object*> objects) {
  BVHBuildNode* node = new BVHBuildNode();

  // Compute bounds of all primitives in BVH node
  Bounds3 bounds;
  for (int i = 0; i < objects.size(); ++i)
    bounds = Union(bounds, objects[i]->GetBounds());
  if (objects.size() == 1) {
    // Create leaf _BVHBuildNode_
    node->bounds = objects[0]->GetBounds();
    node->object = objects[0];
    node->left = nullptr;
    node->right = nullptr;
    return node;
  } else if (objects.size() == 2) {
    node->left = RecursiveBuild(std::vector{objects[0]});
    node->right = RecursiveBuild(std::vector{objects[1]});

    node->bounds = Union(node->left->bounds, node->right->bounds);
    return node;
  } else {
    Bounds3 centroidBounds;
    for (int i = 0; i < objects.size(); ++i)
      centroidBounds = Union(centroidBounds, objects[i]->GetBounds().Centroid());
    int dim = centroidBounds.MaxExtent();
    switch (dim) {
      case 0:
        std::sort(objects.begin(), objects.end(),
                  [](auto f1, auto f2) { return f1->GetBounds().Centroid().x < f2->GetBounds().Centroid().x; });
        break;
      case 1:
        std::sort(objects.begin(), objects.end(),
                  [](auto f1, auto f2) { return f1->GetBounds().Centroid().y < f2->GetBounds().Centroid().y; });
        break;
      case 2:
        std::sort(objects.begin(), objects.end(),
                  [](auto f1, auto f2) { return f1->GetBounds().Centroid().z < f2->GetBounds().Centroid().z; });
        break;
    }

    auto beginning = objects.begin();
    auto middling = objects.begin() + (objects.size() / 2);
    auto ending = objects.end();

    auto leftshapes = std::vector<Object*>(beginning, middling);
    auto rightshapes = std::vector<Object*>(middling, ending);

    assert(objects.size() == (leftshapes.size() + rightshapes.size()));

    node->left = RecursiveBuild(leftshapes);
    node->right = RecursiveBuild(rightshapes);

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

Intersection BVHAccel::GetIntersection(BVHBuildNode* node, const Ray& ray) const {
  // TODO Traverse the BVH to find intersection
}
