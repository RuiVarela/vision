#pragma once

#include "vs.hpp"

namespace vs
{

Mat makeBoxFilter(int w);
Mat makeHighpassFilter();
Mat makeSharpenFilter();
Mat makeEmbossFilter();
Mat makeGaussianFilter(float sigma);
Mat makeSobelFilter(bool horizontal);


void sobel(vs::Mat const& src, vs::Mat& mag, vs::Mat& theta);

void convolve(vs::Mat const& src, vs::Mat& dst, vs::Mat const& filter, bool const preserve = true);
vs::Mat convolve(vs::Mat const& src, vs::Mat const& filter, bool preserve = true);

} // namespace cv
