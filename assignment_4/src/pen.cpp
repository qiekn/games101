#include "pen.h"
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

Pen::Pen(cv::Mat& window) : window_(window) {}

Pen::~Pen() {}

void Pen::add_point(int x, int y) { control_points_.emplace_back(x, y); }

void Pen::move_point(int index, int x, int y) {
  if (index >= control_points_.size() || index < 0) return;
  control_points_[index] = cv::Point2f(x, y);
}

void Pen::set_window(cv::Mat& window) { window_ = window; }

void Pen::draw() {
  draw_instructions();
  bezier();
  draw_control_points();
  draw_control_lines();
}

void Pen::handle_input(int event, int x, int y) {
  if (event == cv::EVENT_LBUTTONDOWN) {  // add  control point
                                         // if (control_points_.size() < 4) {
    std::cout << "add new control point : (" << x << "," << y << ")" << std::endl;
    control_points_.emplace_back(x, y);
    // }
  } else if (event == cv::EVENT_RBUTTONDOWN) {  // select control point
    selected_point_ = get_nearest_point(x, y);
    if (selected_point_ != -1) {
      std::cout << "select point: " << selected_point_ << std::endl;
      is_dragging_ = true;
    }
  } else if (event == cv::EVENT_RBUTTONUP) {  // deselect control point
    if (is_dragging_) {
      std::cout << "deselect point: " << selected_point_ << std::endl;
      is_dragging_ = false;
      selected_point_ = -1;
    }
  } else if (event == cv::EVENT_MOUSEMOVE) {  // move control point
    if (is_dragging_ && selected_point_ != -1) {
      move_point(selected_point_, x, y);
    }
  }
}

const std::vector<cv::Point2f>& Pen::get_control_points() { return control_points_; }

void Pen::draw_control_points() {
  for (int i = 0; i < control_points_.size(); ++i) {
    cv::Scalar color = (i == selected_point_ && is_dragging_)
                           ? cv::Scalar(0, 255, 255)     // when selected, turn color -> yellow
                           : cv::Scalar(255, 255, 255);  // defult color -> white
    cv::circle(window_, control_points_[i], point_radius_, color, -1);

    std::string label = "P" + std::to_string(i);
    cv::putText(window_, label, cv::Point(control_points_[i].x + 10, control_points_[i].y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
  }
}

void Pen::draw_control_lines() {
  if (control_points_.size() <= 2) return;
  for (int i = 0; i < control_points_.size() - 1; ++i) {
    cv::line(window_, control_points_[i], control_points_[i + 1], cv::Scalar(100, 100, 100), 1);
  }
}

void Pen::draw_instructions() {
  // clang-format off
  std::vector<std::string> instructions = {"Instructions:",
                                           "- LButton Click: Add control point",
                                           "- RButton Drag : Move control point",
                                           "- ESC: Exit",
                                           "- S: Save image"};
  // clang-format on

  int y_offset = 20;
  for (const auto& instruction : instructions) {
    cv::putText(window_, instruction, cv::Point(10, y_offset), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(255, 255, 255), 1);
    y_offset += 15;
  }
}

int Pen::get_nearest_point(int x, int y) {
  for (int i = 0; i < control_points_.size(); ++i) {
    float dist = std::pow(x - control_points_[i].x, 2) + std::pow(y - control_points_[i].y, 2);
    if (dist <= std::pow(selected_area_, 2)) return i;
  }
  return -1;
}

void Pen::bezier() {
  // Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's algorithm.
  float step = 0.001f;
  for (float t = 0; t <= 1; t += step) {
    cv::Point2f point;
    {  // Implement de Casteljau's algorithm
      std::vector<cv::Point2f> remain_points = control_points_;
      while (remain_points.size() >= 2) {
        std::vector<cv::Point2f> temp_points;
        for (int i = 0; i < remain_points.size() - 1; ++i) {
          cv::Point2f a = remain_points[i];
          cv::Point2f b = remain_points[i + 1];
          float x = (1 - t) * a.x + t * b.x;
          float y = (1 - t) * a.y + t * b.y;
          temp_points.emplace_back(x, y);
        }
        remain_points.swap(temp_points);
      }
      if (remain_points.size() == 1) point = remain_points.front();
    }
    window_.at<cv::Vec3b>(point.y, point.x)[1] = 255;
  }
}
