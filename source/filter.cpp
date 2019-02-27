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

static inline void convolve(
    int const &src_x, int const &src_y, int const &f_offset,
    const Mat &src, Mat &dst, const Mat &filter,
    bool const &preserve)
{

    float value = 0.0f;

    for (int k = 0; k != src.c; ++k)
    {
        int filter_channel = (src.c == filter.c) ? k : 0;
        
        for (int y = 0; y != filter.h; ++y)
        {
            for (int x = 0; x != filter.w; ++x)
            {
                const float fv = filter.get(x, y, filter_channel);
                const int sx = src_x + x - f_offset;
                const int sy = src_y + y - f_offset;
                const float sv = src.get(sx, sy, k, BorderMode::Clamp);
                value += sv * fv;
            }
        }

        if (preserve)
        {
            dst.set(src_x, src_y, k, value);
            value = 0.0f;
        }
    }

    if (!preserve)
    {
        dst.set(src_x, src_y, 0, value);
    }
}

void convolve(const Mat &src, Mat &dst, const Mat &filter, bool const preserve)
{
    assert((preserve && dst.c == filter.c) || filter.c == 1);

    dst.reshape(src.w, src.h, preserve ? src.c : 1);

    int f_offset = filter.w / 2;
    for (int y = 0; y != src.h; ++y)
        for (int x = 0; x != src.w; ++x)
            convolve(x, y, f_offset, src, dst, filter, preserve);
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
