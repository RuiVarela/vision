#pragma once

#include "vs.hpp"

namespace vs
{

enum ResizeMode
{
    NearestNeighbor,
    Bilinear
};

Mat loadImage(std::string path, int channels = 0);
bool saveImage(std::string path, Mat const &im);

void rgb2gray(Mat const& src, Mat &dst);
Mat rgb2gray(Mat const& src);

void rgb2hsv(Mat const& src, Mat &dst);
Mat rgb2hsv(Mat const& src);
void rgb2hsvInplace(Mat &inplace);

void hsv2rgb(Mat const& src, Mat &dst);
Mat hsv2rgb(Mat const& src);
void hsv2rgbInplace(Mat &inplace);

void resize(Mat const& src, Mat &dst, int nw, int nh, ResizeMode const mode = Bilinear);
Mat resize(Mat const& src, int nw, int nh, ResizeMode const mode = Bilinear);

} // namespace vs
