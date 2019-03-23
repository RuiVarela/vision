#include "vs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace vs
{

float Point::distance(const Point &p, const Point &q)
{
    float x = q.x - p.x;
    float y = q.y - p.y;
    return sqrtf(x * x + y * y);
}

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

    if (channels <= 0)
    {
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

void rgb2gray(Mat const &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 1);
    dst.zero();

    static float scale[] = {0.299f, 0.587f, 0.114f};
    for (int k = 0; k < src.c; ++k)
    {
        for (int j = 0; j < src.h; ++j)
        {
            for (int i = 0; i < src.w; ++i)
            {
                dst.data[i + dst.w * j] += scale[k] * src.get(i, j, k);
            }
        }
    }
}

void rgb2hsv(Mat const &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 3);

    int i, j;
    float r, g, b;
    float h, s, v;
    for (j = 0; j < src.h; ++j)
    {
        for (i = 0; i < src.w; ++i)
        {
            r = src.get(i, j, 0);
            g = src.get(i, j, 1);
            b = src.get(i, j, 2);
            float max = maximum(r, g, b);
            float min = minimum(r, g, b);
            float delta = max - min;
            v = max;

            if (equivalent(delta, 0.0f))
            {
                s = 0.0f;
                h = 0.0f;
            }
            else
            {
                s = delta / max;

                if (equivalent(r, max))
                {
                    h = (g - b) / delta;
                }
                else if (equivalent(g, max))
                {
                    h = 2 + (b - r) / delta;
                }
                else
                {
                    h = 4 + (r - g) / delta;
                }

                if (h < 0)
                    h += 6.0;
                h = h / 6.0f;
            }

            dst.set(i, j, 0, h);
            dst.set(i, j, 1, s);
            dst.set(i, j, 2, v);
        }
    }
}

void hsv2rgb(Mat const &src, Mat &dst)
{
    assert(src.w >= 0 && src.h >= 0 && src.c == 3);
    dst.reshape(src.w, src.h, 3);

    int i, j;
    float r, g, b;
    float h, s, v;
    float f, p, q, t;
    for (j = 0; j < src.h; ++j)
    {
        for (i = 0; i < src.w; ++i)
        {
            h = src.get(i, j, 0) * 6.0f;
            s = src.get(i, j, 1);
            v = src.get(i, j, 2);
            if (equivalent(s, 0.0f))
            {
                r = g = b = v;
            }
            else
            {
                int index = int(floor(h));
                f = h - index;
                p = v * (1 - s);
                q = v * (1 - s * f);
                t = v * (1 - s * (1 - f));
                if (index == 0)
                {
                    r = v;
                    g = t;
                    b = p;
                }
                else if (index == 1)
                {
                    r = q;
                    g = v;
                    b = p;
                }
                else if (index == 2)
                {
                    r = p;
                    g = v;
                    b = t;
                }
                else if (index == 3)
                {
                    r = p;
                    g = q;
                    b = v;
                }
                else if (index == 4)
                {
                    r = t;
                    g = p;
                    b = v;
                }
                else
                {
                    r = v;
                    g = p;
                    b = q;
                }
            }
            dst.set(i, j, 0, r);
            dst.set(i, j, 1, g);
            dst.set(i, j, 2, b);
        }
    }
}

Mat rgb2gray(Mat const &src)
{
    Mat dst;
    rgb2gray(src, dst);
    return dst;
}

Mat rgb2hsv(Mat const &src)
{
    Mat dst;
    rgb2hsv(src, dst);
    return dst;
}

void rgb2hsvInplace(Mat &inplace)
{
    rgb2hsv(inplace, inplace);
}

Mat hsv2rgb(Mat const &src)
{
    Mat dst;
    hsv2rgb(src, dst);
    return dst;
}

void hsv2rgbInplace(Mat &inplace)
{
    hsv2rgb(inplace, inplace);
}

void rgb2bgr(Mat const &src, Mat &dst)
{
    assert(src.c == 3);
    dst.reshape(src.w, src.h, 3);

    for (int i = 0; i < src.w * src.h; ++i)
    {
        float r = src.data[i + src.w * src.h * 0];
        float g = src.data[i + src.w * src.h * 1];
        float b = src.data[i + src.w * src.h * 2];

        dst.data[i + src.w * src.h * 0] = b;
        dst.data[i + src.w * src.h * 1] = g;
        dst.data[i + src.w * src.h * 2] = r;
    }
}

Mat rgb2bgr(Mat const &src)
{
    Mat dst;
    rgb2bgr(src, dst);
    return dst;
}

void rgb2bgrInplace(Mat &inplace)
{
    rgb2bgr(inplace, inplace);
}

float interpolateNN(Mat const &im, float x, float y, int c)
{
    const int ix = int(floorf(x));
    const int iy = int(floorf(y));
    return im.get(ix, iy, c);
}

float interpolateBL(Mat const &im, float x, float y, int c)
{
    // make sure we fall between the pixel center points
    x -= 0.5f;
    y -= 0.5f;

    const int ix = int(floorf(x));
    const int iy = int(floorf(y));

    const float v1 = im.getClamp(ix + 0, iy + 0, c);
    const float v2 = im.getClamp(ix + 1, iy + 0, c);
    const float v3 = im.getClamp(ix + 0, iy + 1, c);
    const float v4 = im.getClamp(ix + 1, iy + 1, c);

    const float d1 = x - ix;
    const float d2 = 1.0f - d1;
    const float d3 = y - iy;
    const float d4 = 1.0f - d3;

    const float q1 = v1 * d2 + v2 * d1;
    const float q2 = v3 * d2 + v4 * d1;
    const float q = q1 * d4 + q2 * d3;

    return q;
}

void resize(Mat const &src, Mat &dst, int nw, int nh, const ResizeMode mode)
{
    dst.reshape(nw, nh, src.c);

    float (*interpolate)(Mat const &, float, float, int) = (mode == Bilinear) ? interpolateBL : interpolateNN;

    float x_ratio = float(src.w) / float(dst.w);
    float y_ratio = float(src.h) / float(dst.h);
    for (int k = 0; k < dst.c; ++k)
    {
        for (int y = 0; y < nh; ++y)
        {
            for (int x = 0; x < nw; ++x)
            {
                float px = (x + 0.5f) * x_ratio;
                float py = (y + 0.5f) * y_ratio;

                float value = interpolate(src, px, py, k);
                dst.set(x, y, k, value);
            }
        }
    }
}

Mat resize(Mat const &src, int nw, int nh, const ResizeMode mode)
{
    Mat dst;
    resize(src, dst, nw, nh, mode);
    return dst;
}

vs::Mat cylindricalProject(vs::Mat const &im, float f)
{
    vs::Mat out(im.w, im.h, im.c);

    float center_x = out.w / 2.0f;
    float center_y = out.h / 2.0f;

    for (int k = 0; k < out.c; ++k)
        for (int y = 0; y < out.h; ++y)
            for (int x = 0; x < out.w; ++x)
            {
                // calculate angle and height
                float angle = (x - center_x) / f;
                float height = (y - center_y) / f;

                // find unit cylindrical coords
                float cylinder_x = sin(angle);
                float cylinder_y = height;
                float cylinder_z = cos(angle);

                // project to image plane
                float px = f * cylinder_x / cylinder_z + center_x;
                float py = f * cylinder_y / cylinder_z + center_y;

                if (px >= 0 && px < im.w && py >= 0 && py < im.h)
                {
                    out.set(x, y, k, im.get(int(px), int(py), k));
                }
            }

    return out;
}

void extractImage4points(Mat const &im, Mat &dst, const std::array<Point, 4> &points)
{
    assert(im.c == dst.c);
    assert(dst.w > 0);
    assert(dst.h > 0);

    float max_x = vs::maximum(points[0].x, points[1].x, points[2].x, points[3].x);
    float max_y = vs::maximum(points[0].y, points[1].y, points[2].y, points[3].y);

    float min_x = vs::minimum(points[0].x, points[1].x, points[2].x, points[3].x);
    float min_y = vs::minimum(points[0].y, points[1].y, points[2].y, points[3].y);
    //tl, tr, bl, br
    std::array<Point, 4> corners = {};

    Mat distances(4, 4);

    /*
     const_image_view<image_type> img(img_);
        image_view<image_type> out(out_);
        if (out.size() == 0)
            return;

        drectangle bounding_box;
        for (auto& p : pts)
            bounding_box += p;

        const std::array<dpoint,4> corners = {{bounding_box.tl_corner(), bounding_box.tr_corner(),
                                               bounding_box.bl_corner(), bounding_box.br_corner()}};

        matrix<double> dists(4,4);
        for (long r = 0; r < dists.nr(); ++r)
        {
            for (long c = 0; c < dists.nc(); ++c)
            {
                dists(r,c) = length_squared(corners[r] - pts[c]);
            }
        }

        matrix<long long> idists = matrix_cast<long long>(-round(std::numeric_limits<long long>::max()*(dists/max(dists))));


        const drectangle area = get_rect(out);
        std::vector<dpoint> from_points = {area.tl_corner(), area.tr_corner(),
                                           area.bl_corner(), area.br_corner()};

        // find the assignment of corners to pts
        auto assignment = max_cost_assignment(idists);
        std::vector<dpoint> to_points(4);
        for (size_t i = 0; i < assignment.size(); ++i)
            to_points[i] = pts[assignment[i]];

        auto tform = find_projective_transform(from_points, to_points);
        transform_image(img_, out_, interpolate_bilinear(), tform);
        */
}

} // namespace vs
