#include "vs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace vs
{

Mat loadImage(std::string path, int channels)
{
    path = toNativeSeparators(path);

    int w, h, c;
    unsigned char *data = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!data)
    {
        std::cerr << "Cannot load image \"" << path << "\" - " << stbi_failure_reason();
        return Mat();
    }

    if (channels <= 0) {
        channels = c;
    }

    Mat im(w, h, channels);
    for (int k = 0; k < channels; ++k)
    {
        for (int j = 0; j < h; ++j)
        {
            for (int i = 0; i < w; ++i)
            {
                int dst_index = i + w * j + w * h * k;
                int src_index = k + c * i + c * w * j;
                im.data[dst_index] = float(data[src_index]) / 255.0f;
            }
        }
    }

    free(data);
    return im;
}

bool saveImage(std::string path, Mat const &im)
{
    path = toNativeSeparators(path);

    int quality = 80;

    unsigned char *data = static_cast<unsigned char *>(calloc(size_t(im.w * im.h * im.c), sizeof(char)));

    for (int k = 0; k < im.c; ++k)
    {
        for (int i = 0; i < im.w * im.h; ++i)
        {
            data[i * im.c + k] = static_cast<unsigned char>(255 * im.data[i + k * im.w * im.h]);
        }
    }

    bool ok = false;
    size_t extension_position = path.size() - 4;
    if (path.rfind(".png") == extension_position)
    {
        ok = stbi_write_png(path.c_str(), im.w, im.h, im.c, data, im.w * im.c) > 0;
    }
    else if (path.rfind(".tga") == extension_position)
    {
        ok = stbi_write_tga(path.c_str(), im.w, im.h, im.c, data) > 0;
    }
    else if (path.rfind(".bmp") == extension_position)
    {
        ok = stbi_write_bmp(path.c_str(), im.w, im.h, im.c, data) > 0;
    }
    else if (path.rfind(".jpg") == extension_position)
    {
        ok = stbi_write_jpg(path.c_str(), im.w, im.h, im.c, data, quality) > 0;
    }

    free(data);

    return ok;
}

void rgb2gray(Mat &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 1);
    dst.zero();

    static float scale[] = {0.299f, 0.587f, 0.114f};
    for(int k = 0; k < src.c; ++k){
        for(int j = 0; j < src.h; ++j){
            for(int i = 0; i < src.w; ++i){
                dst.data[i+dst.w*j] += scale[k] * src.get(i, j, k);
            }
        }
    }
}

void rgb2hsv(Mat &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 3);

    int i, j;
    float r, g, b;
    float h, s, v;
    for(j = 0; j < src.h; ++j){
        for(i = 0; i < src.w; ++i){
            r = src.get(i , j, 0);
            g = src.get(i , j, 1);
            b = src.get(i , j, 2);
            float max = maximum(r,g,b);
            float min = minimum(r,g,b);
            float delta = max - min;
            v = max;

            if (equivalent(delta, 0.0f)) {
                s = 0.0f;
                h = 0.0f;
            } else {
                s = delta / max;

                if(equivalent(r, max)){
                    h = (g - b) / delta;
                } else if (equivalent(g, max)) {
                    h = 2 + (b - r) / delta;
                } else {
                    h = 4 + (r - g) / delta;
                }

                if (h < 0) h += 6.0;
                h = h/6.0f;
            }

            dst.set(i, j, 0, h);
            dst.set(i, j, 1, s);
            dst.set(i, j, 2, v);
        }
    }
}

void hsv2rgb(Mat &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 3);

    int i, j;
    float r, g, b;
    float h, s, v;
    float f, p, q, t;
    for(j = 0; j < src.h; ++j){
        for(i = 0; i < src.w; ++i){
            h = src.get(i , j, 0) * 6.0f;
            s = src.get(i , j, 1);
            v = src.get(i , j, 2);
            if (equivalent(s, 0.0f)) {
                r = g = b = v;
            } else {
                int index = int(floor(h));
                f = h - index;
                p = v*(1-s);
                q = v*(1-s*f);
                t = v*(1-s*(1-f));
                if(index == 0){
                    r = v; g = t; b = p;
                } else if(index == 1){
                    r = q; g = v; b = p;
                } else if(index == 2){
                    r = p; g = v; b = t;
                } else if(index == 3){
                    r = p; g = q; b = v;
                } else if(index == 4){
                    r = t; g = p; b = v;
                } else {
                    r = v; g = p; b = q;
                }
            }
            dst.set(i, j, 0, r);
            dst.set(i, j, 1, g);
            dst.set(i, j, 2, b);
        }
    }

}


Mat rgb2gray(Mat &src)
{
    Mat dst;
    rgb2gray(src, dst);
    return dst;
}


Mat rgb2hsv(Mat &src)
{
    Mat dst;
    rgb2hsv(src, dst);
    return dst;
}

void rgb2hsvInplace(Mat &inplace)
{
    rgb2hsv(inplace, inplace);
}

Mat hsv2rgb(Mat &src)
{
    Mat dst;
    hsv2rgb(src, dst);
    return dst;
}

void hsv2rgbInplace(Mat &inplace)
{
    hsv2rgb(inplace, inplace);
}

float bilinear_interpolate(Mat const& im, float x, float y, int c)
{
    const BorderMode mode = BorderMode::Clamp;

    const int ix = int(floorf(x));
    const int iy = int(floorf(y));

    const float v1 = im.get(ix + 0, iy + 0, c, mode);
    const float v2 = im.get(ix + 1, iy + 0, c, mode);
    const float v3 = im.get(ix + 0, iy + 1, c, mode);
    const float v4 = im.get(ix + 1, iy + 1, c, mode);

    const float d1 = x - ix;
    const float d2 = 1.0f - d1;
    const float d3 = y - iy;
    const float d4 = 1.0f - d3;

    const float q1 = v1*d2 + v2*d1;
    const float q2 = v3*d2 + v4*d1;
    const float q  = q1*d4 + q2*d3;

    return q;
}

void resize(Mat &src, Mat &dst, int nw, int nh, const ResizeMode mode)
{
    dst.reshape(nw, nh, src.c);


    if (mode == Bilinear) {

        float x_ratio = float(src.w + 1.0) / float(dst.w);
        float y_ratio = float(src.h + 1.0) / float(dst.h);
        for (int k = 0; k < dst.c; ++k)
        {
            for (int y = 0; y < nh; ++y)
            {
                for (int x = 0; x < nw; ++x)
                {
                    float px = (x + 0.5f) * x_ratio - 0.5f - 1.0f;
                    float py = (y + 0.5f) * y_ratio - 0.5f - 1.0f;

                    float value = bilinear_interpolate(src, px, py, k);
                    dst.set(x, y, k, value);
                }
            }
        }

    } else if  (mode == NearestNeighbor) {

        float x_ratio = float(src.w) / float(dst.w);
        float y_ratio = float(src.h) / float(dst.h);
        for (int k = 0; k < dst.c; ++k)
        {
            for (int y = 0; y < nh; ++y)
            {
                for (int x = 0; x < nw; ++x)
                {
                    int sx = int(floorf((x + 0.5f) * x_ratio));
                    int sy = int(floorf((y + 0.5f) * y_ratio));
                    dst.set(x, y, k, src.get(sx, sy, k));
                }
            }
        }
    }

}

Mat resize(Mat &src, int nw, int nh, const ResizeMode mode)
{
    Mat dst;
    resize(src, dst, nw, nh, mode);
    return dst;
}

} // namespace vs
