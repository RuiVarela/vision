#pragma once

namespace vs
{

class Mat
{
  public:
    Mat();
    Mat(int w, int h = 1, int c = 1);
    ~Mat();

    void reshape(int w, int h, int c);
    int size();

    std::ostream& print(std::ostream& out, int max_cols = 0, int max_rows = 0);

    int w;
    int h;
    int c;
    float *data;

  private:
    std::shared_ptr<float> shared_data;
};

Mat loadImage(std::string const &path);
bool saveImage(std::string const &path, Mat const &im);

} // namespace cv
