#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "pen.h"

bool draw_naive_bezier = true;

void mouse_handler(int event, int x, int y, int flags, void *userdata) {
  Pen *pen = static_cast<Pen *>(userdata);
  if (!pen) return;
  pen->handle_input(event, x, y);
  if (event == cv::EVENT_FLAG_SHIFTKEY) {
    draw_naive_bezier = !draw_naive_bezier;
  }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) {
  if (points.size() != 4) return;
  auto &p_0 = points[0];
  auto &p_1 = points[1];
  auto &p_2 = points[2];
  auto &p_3 = points[3];

  for (double t = 0.0; t <= 1.0; t += 0.001) {
    auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;
    window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
  }
}

int main() {
  cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));  // init window
  cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

  Pen pen(window);
  cv::setMouseCallback("Bezier Curve", mouse_handler, &pen);  // handle input

  int key = -1;
  while (key != 27) {
    window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));  // clear background
    pen.set_window(window);
    pen.draw();
    if (draw_naive_bezier) naive_bezier(pen.get_control_points(), window);
    cv::imshow("Bezier Curve", window);

    key = cv::waitKey(20);
    if (key == 's' || key == 'S') {
      cv::imwrite("my_bezier_curve.png", window);
      std::cout << "saved to ./my_bezier_curve.png" << std::endl;
    }
  }
  return 0;
}
