#pragma once

#include "vs.hpp"

namespace vs
{

Mat makeHighpassFilter();
Mat makeSharpenFilter();
Mat makeEmbossFilter();


Mat makeBoxFilter(int w);
Mat makeGaussianFilter(float sigma);
Mat makeGaussianFilter1D(float sigma);
void smoothImage(vs::Mat const& src, vs::Mat& dst, vs::Mat& tmp, float sigma);
void smoothImage(vs::Mat const& src, vs::Mat& dst, float sigma);
vs::Mat smoothImage(vs::Mat const& src, float sigma);


Mat makeSobelFilter(bool horizontal);
void gradientGray(vs::Mat const& src, vs::Mat& gx, vs::Mat& gy);
void gradient(vs::Mat const& src, vs::Mat& gx, vs::Mat& gy);
void sobel(vs::Mat const& src, vs::Mat& mag, vs::Mat& theta);


void convolve(vs::Mat const& src, vs::Mat& dst, vs::Mat const& filter, bool const preserve = true);
vs::Mat convolve(vs::Mat const& src, vs::Mat const& filter, bool preserve = true);

} // namespace cv
