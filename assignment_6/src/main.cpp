#include <chrono>
#include "renderer.h"
#include "scene.h"
#include "triangle.h"
#include "vector.h"

// In the main function of the program, we create the scene (create objects and lights) as well as
// set the options for the render (image width and height, maximum recursion depth, field-of-view,
// etc.). We then call the render function().

int main() {
  Scene scene(1280, 960);

  MeshTriangle bunny("../models/bunny.obj");

  scene.Add(&bunny);
  scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 1));
  scene.Add(std::make_unique<Light>(Vector3f(20, 70, 20), 1));
  scene.BuildBVH();

  Renderer renderer;

  // see: https://qiekn.notion.site/cpp-benchmarking
  using clock = std::chrono::steady_clock;

  auto start = clock::now();
  renderer.Render(scene);
  auto stop = clock::now();

  auto elapsed = stop - start;
  std::chrono::hh_mm_ss hms{elapsed};

  // clang-format off
  std::cout << "Render complete: \n";
  std::cout << "Time taken: " 
            << hms.hours().count() << "h "
            << hms.minutes().count() << "m "
            << hms.seconds().count() << "s\n";
  // clang-format on

  return 0;
}
