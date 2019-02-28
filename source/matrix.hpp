#pragma once

#include "vs.hpp"

namespace vs
{

enum BorderMode
{
  Zero,
  Clamp
};

class Mat
{
public:
  Mat();
  explicit Mat(int w, int h = 1, int c = 1);

  void reshape(int w, int h, int c);
  int size() const;
  Mat clone() const;

  Mat &zero();
  Mat &fill(int c, float v);
  Mat &fill(float v);

  float get(int x, int y = 0, int c = 0) const;
  float get(int x, int y, int c, BorderMode border) const;
  Mat &set(int x, int y, int c, float v);

  // clamp all values between [min, max]
  Mat &clamp(int c, float min, float max);
  Mat &clamp(float min, float max);
  Mat &clamp(); // 0.0f, 1.0f

  // Add a scalar value
  Mat &add(int c, float v);
  Mat &add(float v);
  // Add a mat
  Mat &add(Mat const &v);
  static Mat add(Mat const &a, Mat const &b);

  // Subtract a mat
  Mat &sub(Mat const &v);
  static Mat sub(Mat const &a, Mat const &b);

  // Multiply by a scalar
  Mat &mult(int c, float v);
  Mat &mult(float v);

  // sums all values in a channel
  float sum(int c);

  // normalizes a channels using l1norm
  Mat &l1Normalize(int c);
  Mat &l1Normalize();

  Mat &featureNormalize(int c);
  Mat &featureNormalize();

  int w;
  int h;
  int c;
  float *data;

private:
  std::shared_ptr<float> shared_data;
};

} // namespace vs
