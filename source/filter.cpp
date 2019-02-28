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
    Mat filter(3, 3, 1);
    filter
            .set(0,0,0,  0.0f).set(1,0,0, -1.0f).set(2,0,0,  0.0f)
            .set(0,1,0, -1.0f).set(1,1,0,  4.0f).set(2,1,0, -1.0f)
            .set(0,2,0,  0.0f).set(1,2,0, -1.0f).set(2,2,0,  0.0f);

    return filter;
}

Mat makeSharpenFilter()
{
    Mat filter(3, 3, 1);
    filter
            .set(0,0,0,  0.0f).set(1,0,0, -1.0f).set(2,0,0,  0.0f)
            .set(0,1,0, -1.0f).set(1,1,0,  5.0f).set(2,1,0, -1.0f)
            .set(0,2,0,  0.0f).set(1,2,0, -1.0f).set(2,2,0,  0.0f);

    return filter;
}

Mat makeEmbossFilter()
{
    Mat filter(3, 3, 1);
    filter
            .set(0,0,0, -2.0f).set(1,0,0, -1.0f).set(2,0,0,  0.0f)
            .set(0,1,0, -1.0f).set(1,1,0,  1.0f).set(2,1,0,  1.0f)
            .set(0,2,0,  0.0f).set(1,2,0,  1.0f).set(2,2,0,  2.0f);

    return filter;
}

Mat makeGaussianFilter(float sigma)
{
    assert(sigma > 1.0f);

    int k = 6 * int(floorf(sigma));
    if (k % 2 == 0) {
        k++;
    }
    int const offset = k / 2;

    Mat dst(k , k , 1);
    for (int y = 0; y != dst.h; ++y) {
        for (int x = 0; x != dst.w; ++x) {
            int const fx = x - offset;
            int const fy = y - offset;
            float const g = (1.0f / (2.0f * float(M_PI) * sigma * sigma)) * exp(- (fx*fx + fy*fy) / (2.0f * sigma * sigma));
            dst.set(x, y, 0, g);
        }
    }

    return dst;
}

void sobel(const Mat &src, Mat &mag, Mat &theta)
{

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

} // namespace cv
