#pragma once

#include "vs.hpp"

namespace vs
{


Mat makeBoxFilter(int w)
{
    // TODO
    return Mat();
}

Mat makeHighpassFilter()
{
     // TODO
    return Mat();
}

Mat makeSharpenFilter()
{
     // TODO
    return Mat();
}

Mat makeEmbossFilter()
{
     // TODO
    return Mat();
}


void convolve(const Mat &src, Mat &dst, const Mat &filter, bool preserve)
{

}

Mat convolve(const Mat &src, const Mat &filter, bool preserve)
{
    Mat dst;
    convolve(src, dst, filter, preserve);
    return dst;
}

Mat makeGaussianFilter(float sigma)
{
    return Mat();
}

void sobel(const Mat &src, Mat &mag, Mat &theta)
{

}







} // namespace cv
