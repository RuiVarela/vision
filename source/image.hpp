#pragma once

namespace vs
{

Mat loadImage(std::string path, int channels = 0);
bool saveImage(std::string path, Mat const &im);

void rgb2gray(Mat &src, Mat &dst);
Mat rgb2gray(Mat &src);


void rgb2hsv(Mat &src, Mat &dst);
Mat rgb2hsv(Mat &src);
void rgb2hsvInplace(Mat &inplace);


void hsv2rgb(Mat &src, Mat &dst);
Mat hsv2rgb(Mat &src);
void hsv2rgbInplace(Mat &inplace);




} // namespace cv
