#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

class Pen {
public:
  Pen(cv::Mat& window);
  virtual ~Pen();

  void add_point(int x, int y);
  void move_point(int index, int x, int y);
  void handle_input(int event, int x, int y);
  void set_window(cv::Mat& window);
  void draw();
  const std::vector<cv::Point2f>& get_control_points();

private:
  void draw_control_points();
  void draw_control_lines();
  void draw_instructions();
  void bezier();
  int get_nearest_point(int x, int y);

private:
  /* data */
  std::vector<cv::Point2f> control_points_;
  cv::Mat window_;
  bool is_dragging_ = false;

  int selected_point_ = -1;
  int selected_area_ = 8;  // Collision detection range
  int point_radius_ = 3;
};
