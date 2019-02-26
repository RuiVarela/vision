#pragma once

namespace vs
{

class Mat
{
  public:
    Mat();
    Mat(int w, int h = 1, int c = 1);

    void reshape(int w, int h, int c);
    int size();
    Mat clone();
    Mat& zero();

    float get(int x, int y, int c);
    Mat& set(int x, int y, int c, float v);

    Mat& add(int c, float v);
    Mat& add(float v);

    Mat& mult(int c, float v);
    Mat& mult(float v);

    Mat &clamp(int c, float min, float max);
    Mat& clamp(float min, float max);

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
