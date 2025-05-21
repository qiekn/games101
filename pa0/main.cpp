#include <cmath>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

int main() {
  using std::cout;
  using std::endl;
  // Basic Example of cpp
  cout << endl;
  cout << "/*─────────────────────────────────────┐ " << endl;
  cout << "│            Example of cpp            │ " << endl;
  cout << "└──────────────────────────────────────*/" << endl;

  float a = 1.0, b = 2.0;
  cout << "a = " << a << endl;
  cout << "b = " << b << endl;
  cout << "a / b = " << a / b << endl;
  cout << "sqrt(b) = " << sqrt(b) << endl;
  cout << "acos(-1) = " << acos(-1) << endl;
  cout << "sin(π/6) = " << sin(30.0 / 180.0 * acos(-1)) << endl;

  // Example of vector
  cout << endl;
  cout << "/*─────────────────────────────────────┐ " << endl;
  cout << "│          Example of vector           │ " << endl;
  cout << "└──────────────────────────────────────*/" << endl;

  // vector definition
  Eigen::Vector3f v(1.0f, 2.0f, 3.0f);
  Eigen::Vector3f w(1.0f, 0.0f, 0.0f);

  // vector output
  cout << "matrix v output\n";
  cout << v << endl;

  cout << "matrix w output\n";
  cout << w << endl;

  // vector add
  cout << "vector v + w output\n";
  cout << v + w << endl;

  // vector scalar multiply
  cout << "vector scalar multiply v * 3.0f\n";
  cout << v * 3.0f << endl;

  cout << "vector scalar multiply 2.0f * v\n";
  cout << 2.0f * v << endl;

  // Example of matrix
  cout << endl;
  cout << "/*─────────────────────────────────────┐ " << endl;
  cout << "│          Example of matrix           │ " << endl;
  cout << "└──────────────────────────────────────*/" << endl;

  // matrix definition
  Eigen::Matrix3f i, j;
  i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
  j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;

  // matrix output
  cout << "matrx i" << endl;
  cout << i << "\n\n";

  cout << "matrx j" << endl;
  cout << j << "\n\n";

  // matrix add i + j
  cout << "matrix add i + j" << endl;
  cout << i + j << "\n\n";

  // matrix scalar multiply i * 2.0
  cout << "matrix add i * 2.0" << endl;
  cout << i * 2.0 << "\n\n";

  // matrix multiply i * j
  cout << "matrix add i * j" << endl;
  cout << i * j << "\n\n";

  // matrix multiply vector i * v
  cout << "matrix add i * v" << endl;
  cout << i * v << endl;

  cout << endl;
  cout << "/*─────────────────────────────────────┐ " << endl;
  cout << "│               Homework               │ " << endl;
  cout << "└──────────────────────────────────────*/" << endl;

  Eigen::Vector3f p(2.0f, 1.0f, 1.0f);
  cout << "p before translate:\n" << p << endl;

  Eigen::Matrix3f homework_translate_matrix;
  // homework note

  // rotate matrix
  // cos(45°) -sin(45°)
  // sin(45°) cos(45°)

  // final translation
  // cos(45°) -sin(45°) 2
  // sin(45°) cos(45°)  1
  // 0        0         1

  // clang-format off
  float theta = 45.0 / 180.0 * acos(-1);
  homework_translate_matrix
    << cos(theta), -sin(theta), 1,
       sin(theta),  cos(theta), 2,
       0, 0, 1;
  // clang-format on

  p = homework_translate_matrix * p;
  cout << "p after translate:\n" << p << endl;
  return 0;
}
