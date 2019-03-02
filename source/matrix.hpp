#pragma once

#include "vs.hpp"

namespace vs
{

class Mat
{
public:
  Mat();
  explicit Mat(int w, int h = 1, int c = 1);
  explicit Mat(int w, int h, int c, float* ext);

  void reshape(int w, int h, int c);
  int size() const;
  int channelSize() const;
  Mat clone() const;

  Mat &zero();
  Mat &fill(int c, float v); // fills a channel with v value
  Mat &fill(float v); // fills all channels with v value
  Mat &fill(Mat const& src, int src_c, int dst_c); // fills a channel with values from another mat

  float get(int x, int y = 0, int c = 0) const;
  float getClamp(int x, int y, int c) const;
  float getZero(int x, int y, int c) const;
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

  
  float sum(int c); // sums all values in a channel
  float max(int c); // max value in a channel
  float min(int c); // min value in a channel
  void minNmax(int c, float& minv, float& maxv); // min and max value in a channel


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
