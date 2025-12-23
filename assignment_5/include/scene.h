#pragma once

#include <memory>
#include <vector>
#include "light.h"
#include "object.h"
#include "vector.h"

class Scene {
public:
  // setting up options
  int width = 1280;
  int height = 960;
  double fov = 90;
  Vector3f background_color = Vector3f(0.235294, 0.67451, 0.843137);
  int max_depth = 5;
  float epsilon = 0.00001;

  Scene(int w, int h) : width(w), height(h) {}

  void Add(std::unique_ptr<Object> object) { objects_.push_back(std::move(object)); }
  void Add(std::unique_ptr<Light> light) { lights_.push_back(std::move(light)); }

  [[nodiscard]] const std::vector<std::unique_ptr<Object> >& GetObjects() const { return objects_; }
  [[nodiscard]] const std::vector<std::unique_ptr<Light> >& GetLights() const { return lights_; }

private:
  // creating the scene (adding objects and lights)
  std::vector<std::unique_ptr<Object> > objects_;
  std::vector<std::unique_ptr<Light> > lights_;
};
