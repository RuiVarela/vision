#include "vs.hpp"

namespace vs
{



Mat makeHighpassFilter()
{
    Mat filter(3, 3, 1);
    float* f = filter.data;
    (*f++) =  0.0f; (*f++) = -1.0f; (*f++) =  0.0f;
    (*f++) = -1.0f; (*f++) =  4.0f; (*f++) = -1.0f;
    (*f++) =  0.0f; (*f++) = -1.0f; (*f++) =  0.0f;

    // filter
    //         .set(0,0,0,  0.0f).set(1,0,0, -1.0f).set(2,0,0,  0.0f)
    //         .set(0,1,0, -1.0f).set(1,1,0,  4.0f).set(2,1,0, -1.0f)
    //         .set(0,2,0,  0.0f).set(1,2,0, -1.0f).set(2,2,0,  0.0f);

    return filter;
}

Mat makeSharpenFilter()
{
    Mat filter(3, 3, 1);
    float* f = filter.data;
    (*f++) =  0.0f; (*f++) = -1.0f; (*f++) =  0.0f;
    (*f++) = -1.0f; (*f++) =  5.0f; (*f++) = -1.0f;
    (*f++) =  0.0f; (*f++) = -1.0f; (*f++) =  0.0f;
    return filter;
}

Mat makeEmbossFilter()
{
    Mat filter(3, 3, 1);
    float* f = filter.data;
    (*f++) = -2.0f; (*f++) = -1.0f; (*f++) =  0.0f;
    (*f++) = -1.0f; (*f++) =  1.0f; (*f++) =  1.0f;
    (*f++) =  0.0f; (*f++) =  1.0f; (*f++) =  2.0f;
    return filter;
}

Mat makeBoxFilter(int w)
{
    Mat filter(w, w, 1);
    float value = 1.0f / (w * w);
    return filter.fill(value);
}


Mat makeGaussianFilter(float sigma)
{
    assert(sigma > 0.0f);

    int k =  int(floorf(6 * sigma));
    if (k % 2 == 0) {
        k++;
    }
    int const offset = k / 2;

    float sigma2 = 2.0f * sigma * sigma;
    float norm = 1.0f / (float(M_PI) * sigma2);

    Mat dst(k, k, 1);
    for (int y = 0; y != dst.h; ++y) {
        for (int x = 0; x != dst.w; ++x) {
            int const fx = x - offset;
            int const fy = y - offset;
            float const g = norm  * exp(- (fx*fx + fy*fy) / sigma2);
            dst.set(x, y, 0, g);
        }
    }

    return dst;
}

Mat makeGaussianFilter1D(float sigma)
{
    assert(sigma > 0.0f);

    int k = int(floorf(6 * sigma));
    if (k % 2 == 0) {
        k++;
    }
    int const offset = k / 2;

    float sigma2 = 2.0f * sigma * sigma;
    float norm = 1.0f / (sqrt(2.0f * float(M_PI)) * sigma);

    Mat dst(k, 1, 1);
    for (int x = 0; x != dst.w; ++x) {
        int const fx = x - offset;
        float const g = norm * exp(- (fx*fx) / sigma2);
        dst.set(x, 0, 0, g);
    }

    return dst;
}

void smoothImage(vs::Mat const& src, vs::Mat& dst, vs::Mat& tmp, float sigma) {
    //
    // expensive way, convolve with 2d gaussian filter
    //
    //Mat f = vs::makeGaussianFilter(sigma);
    //convolve(src, dst, f);

    //
    // faster convolve with 1d horizontal filter and then with the vertical filter
    //
    Mat f = makeGaussianFilter1D(sigma);
    convolve(src, tmp, f);
    std::swap(f.w, f.h);
    convolve(tmp, dst, f);
}

void smoothImage(vs::Mat const& src, vs::Mat& dst, float sigma) {
    Mat tmp;
    smoothImage(src, dst, tmp, sigma);
}

vs::Mat smoothImage(vs::Mat const& src, float sigma) {
    Mat output;
    smoothImage(src, output, sigma);
    return output;
}


Mat makeSobelFilter(bool horizontal)
{
    Mat filter(3, 3, 1);
    if (horizontal) {
        float* f = filter.data;
        (*f++) = -1.0f; (*f++) =  0.0f; (*f++) =  1.0f;
        (*f++) = -2.0f; (*f++) =  0.0f; (*f++) =  2.0f;
        (*f++) = -1.0f; (*f++) =  0.0f; (*f++) =  1.0f;
    } else {
        float* f = filter.data;
        (*f++) = -1.0f; (*f++) = -2.0f; (*f++) = -1.0f;
        (*f++) =  0.0f; (*f++) =  0.0f; (*f++) =  0.0f;
        (*f++) =  1.0f; (*f++) =  2.0f; (*f++) =  1.0f;
    }
    return filter;
}

void gradientGray(const Mat &src, Mat &gx, Mat &gy)
{
    assert(src.c == 1);

    float* f;
    Mat filter(3, 1, 1);
    Mat tmp;

    //
    // gy
    //
    f = filter.data;
    (*f++) = -1.0f; (*f++) =  0.0f; (*f++) =  1.0f;
    convolve(src, tmp, filter);

    std::swap(filter.w, filter.h);
    f = filter.data;
    (*f++) =  1.0f; (*f++) =  2.0f; (*f++) =  1.0f;
    convolve(tmp, gx, filter);

    //
    // gy
    //
    std::swap(filter.w, filter.h);
    f = filter.data;
    (*f++) =  1.0f; (*f++) =  2.0f; (*f++) =  1.0f;
    convolve(src, tmp, filter);

    std::swap(filter.w, filter.h);
    f = filter.data;
    (*f++) = -1.0f; (*f++) =  0.0f; (*f++) =  1.0f;
    convolve(tmp, gy, filter);
}

void gradient(vs::Mat const& src, vs::Mat& gx, vs::Mat& gy) {
    //
    // expensive way, convolve with 2d gaussian filter
    //
    gx = convolve(src, makeSobelFilter(true), false);
    gy = convolve(src, makeSobelFilter(false), false);
}

void sobel(const Mat &src, Mat &mag, Mat &theta)
{
    Mat gx, gy;
    gradient(src, gx, gy);

    mag.reshape(src.w, src.h, 1);
    theta.reshape(src.w, src.h, 1);

    for (int i = 0; i != gx.w * gx.h; ++i)
    {
        float gx_v = gx.data[i];
        float gy_v = gy.data[i];
        mag.data[i] = sqrtf(gx_v * gx_v + gy_v * gy_v);
        theta.data[i] = atan2(gy_v, gx_v);
    }
}

static inline void convolve(
    int const &src_x, int const &src_y, int const &fx_offset, int const &fy_offset,
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
                const int sx = src_x + x - fx_offset;
                const int sy = src_y + y - fy_offset;
                const float sv = src.getClamp(sx, sy, k);
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

    int fx_offset = filter.w / 2;
    int fy_offset = filter.h / 2;
    for (int y = 0; y != src.h; ++y)
        for (int x = 0; x != src.w; ++x)
            convolve(x, y, fx_offset, fy_offset, src, dst, filter, preserve);
}

Mat convolve(const Mat &src, const Mat &filter, bool preserve)
{
    Mat dst;
    convolve(src, dst, filter, preserve);
    return dst;
}



} // namespace cv
