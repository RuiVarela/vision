#pragma once

#include "vs.hpp"

namespace vs
{

class Mat
{
  public:
    Mat();
    Mat(int w, int h = 1, int c = 1);

    void reshape(int w, int h, int c);
    int size() const;
    Mat clone() const;
    Mat& zero();

    float get(int x, int y, int c);
    Mat& set(int x, int y, int c, float v);

    Mat& add(int c, float v);
    Mat& add(float v);
    Mat &add(Mat const& v);
    static Mat add(Mat const& a, Mat const& b);

    Mat &sub(Mat const& v);
    static Mat sub(Mat const& a, Mat const& b);

    Mat& mult(int c, float v);
    Mat& mult(float v);

    Mat& l1Normalize(int c);
    Mat& l1Normalize();

    Mat& featureNormalize(int c);
    Mat& featureNormalize();

    Mat &clamp(int c, float min, float max);
    Mat& clamp(float min, float max);
    Mat& clamp(); // 0.0f, 1.0f

    std::ostream& print(std::ostream& out, int max_cols = 0, int max_rows = 0);

    int w;
    int h;
    int c;
    float *data;

  private:
    std::shared_ptr<float> shared_data;
};

bool sameMat(Mat const& a, Mat const& b);

} // namespace cv
