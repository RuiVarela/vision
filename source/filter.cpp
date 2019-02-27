#include "vs.hpp"

namespace vs
{

Mat makeBoxFilter(int w)
{
    Mat filter(w, w, 1);
    float value = 1.0f / (w * w);
    return filter.fill(value);
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
    assert((preserve && dst.c == filter.c) || filter.c == 1);


    if (!preserve) {
        assert(false);
    }



    int filter_channel = 0;



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
