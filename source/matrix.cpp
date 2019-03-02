#include "vs.hpp"

namespace vs
{

Mat::Mat()
    : w(0), h(0), c(0), data(nullptr)
{
}

Mat::Mat(int w, int h, int c)
    : Mat()
{
    reshape(w, h, c);
}

Mat::Mat(int w, int h, int c, float *ext)
    : Mat()
{
    assert(ext != nullptr);

    this->w = w;
    this->h = h;
    this->c = c;
    this->data = ext;
}

Mat Mat::clone() const
{
    Mat output(w, h, c);
    memcpy(output.data, data, size_t(size()) * sizeof(float));
    return output;
}

Mat &Mat::zero()
{
    memset(data, 0, size_t(size()) * sizeof(float));
    return *this;
}

Mat &Mat::fill(int c, float v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = v;

    return *this;
}

Mat &Mat::fill(float v)
{
    for (int k = 0; k != c; ++k)
        fill(k, v);

    return *this;
}

Mat &Mat::fill(Mat const &src, int src_c, int dst_c)
{
    assert(src.w == w && src.h == h && src.c > src_c && c > dst_c);

    for (int i = 0; i != w * h; ++i)
        data[dst_c * w * h + i] = src.data[src_c * w * h + i];
    
    return *this;
}

float Mat::get(int x, int y, int c) const
{
    assert(x >= 0 && x < this->w && y >= 0 && y < this->h && c >= 0 && c < this->c);
    return data[c * h * w + y * w + x];
}

float Mat::getClamp(int x, int y, int c) const
{
    x = vs::clampTo(x, 0, this->w - 1);
    y = vs::clampTo(y, 0, this->h - 1);
    c = vs::clampTo(c, 0, this->c - 1);
    return data[c * h * w + y * w + x];
}

float Mat::getZero(int x, int y, int c) const
{
    if (x < 0 || x >= this->w || y < 0 || y >= this->h || c < 0 || c >= this->c)
        return 0;

    return data[c * h * w + y * w + x];

}

Mat &Mat::set(int x, int y, int c, float v)
{
    assert(x >= 0 && y >= 0 && c >= 0);
    assert(x < w && y < h && c < this->c);

    data[c * h * w + y * w + x] = v;

    return *this;
}

Mat &Mat::add(int c, float v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] += v;

    return *this;
}

Mat &Mat::add(float v)
{
    for (int k = 0; k != c; ++k)
        add(k, v);

    return *this;
}

Mat &Mat::add(const Mat &v)
{
    assert(c == v.c && w == v.w && h == v.h);

    for (int i = 0; i != c * w * h; ++i)
        data[i] += v.data[i];

    return *this;
}

Mat Mat::add(const Mat &a, const Mat &b)
{
    Mat output = a.clone();
    output.add(b);
    return output;
}

Mat &Mat::sub(const Mat &v)
{
    assert(c == v.c && w == v.w && h == v.h);

    for (int i = 0; i != c * w * h; ++i)
        data[i] -= v.data[i];

    return *this;
}

Mat Mat::sub(const Mat &a, const Mat &b)
{
    Mat output = a.clone();
    output.sub(b);
    return output;
}

Mat &Mat::mult(int c, float v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] *= v;

    return *this;
}

Mat &Mat::mult(float v)
{
    for (int k = 0; k != c; ++k)
        mult(k, v);

    return *this;
}

float Mat::sum(int c)
{
    float value = 0.0;

    for (int i = 0; i != w * h; ++i)
        value += data[c * h * w + i];

    return value;
}

float Mat::max(int c)
{
    float value = std::numeric_limits<float>::min();

    for (int i = 0; i != w * h; ++i) {
        float current = data[c * h * w + i];
        if (current > value) {
            value = current;
        }
    }

    return value;
}

float Mat::min(int c)
{
    float value = std::numeric_limits<float>::max();

    for (int i = 0; i != w * h; ++i) {
        float current = data[c * h * w + i];
        if (current < value) {
            value = current;
        }
    }

    return value;
}

void Mat::minNmax(int c, float &minv, float &maxv)
{
    minv = std::numeric_limits<float>::max();
    maxv = std::numeric_limits<float>::min();

    for (int i = 0; i != w * h; ++i)
    {
        float current = data[c * h * w + i];
        if (current < minv)
        {
            minv = current;
        }

        if (current > maxv)
        {
            maxv = current;
        }
    }
}

Mat &Mat::featureNormalize(int c)
{
    float max_v;
    float min_v;
    minNmax(c, min_v, max_v);

    float delta = max_v - min_v;

    if (equivalent(delta, 0.0f))
        return zero();

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = (data[c * h * w + i] - min_v) / delta;

    return *this;
}

Mat &Mat::featureNormalize()
{
    for (int k = 0; k != c; ++k)
        featureNormalize(k);

    return *this;
}

Mat &Mat::l1Normalize(int c)
{
    float channel_sum = sum(c);
    float value = 1.0f / channel_sum;
    return mult(c, value);
}

Mat &Mat::l1Normalize()
{
    for (int k = 0; k != c; ++k)
        l1Normalize(k);

    return *this;
}

Mat &Mat::clamp(int c, float min, float max)
{
    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = vs::clampTo(data[c * h * w + i], min, max);

    return *this;
}

Mat &Mat::clamp(float min, float max)
{
    for (int k = 0; k != c; ++k)
        clamp(k, min, max);

    return *this;
}

Mat &Mat::clamp()
{
    return clamp(0.0f, 1.0f);
}

int Mat::size() const
{
    return w * h * c;
}

int Mat::channelSize() const {
    return w * h;  
}

void Mat::reshape(int w, int h, int c)
{
    if (this->w == w && this->h == h && this->c == c)
    {
        return;
    }

    shared_data.reset();
    data = nullptr;
    this->w = 0;
    this->h = 0;
    this->c = 0;

    if (w > 0 && h > 0 && c > 0)
    {
        this->w = w;
        this->h = h;
        this->c = c;

        data = static_cast<float *>(calloc(size_t(size()), sizeof(float)));
        if (data)
        {
            shared_data = std::shared_ptr<float>(data, free);
        }
    }
}

} // namespace vs
