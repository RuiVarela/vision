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

Mat Mat::channelView(int c, int count)
{
    assert(this->c > (c + count - 1));

    Mat output(w, h, count, data + c * channelSize());
    // share the data across mat objets
    output.shared_data = shared_data;

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

Mat &Mat::setClamp(int x, int y, int c, float v)
{
    x = vs::clampTo(x, 0, this->w - 1);
    y = vs::clampTo(y, 0, this->h - 1);
    c = vs::clampTo(c, 0, this->c - 1);
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

Mat Mat::transpose() const
{
    Mat t(h, w, c);

    for (int k = 0; k != t.c; ++k)
        for (int y = 0; y != t.h; ++y)
            for (int x = 0; x != t.w; ++x)
                t.set(x, y, k, get(y, x, k));

    return t;
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

    for (int i = 0; i != w * h; ++i)
    {
        float current = data[c * h * w + i];
        if (current > value)
        {
            value = current;
        }
    }

    return value;
}

float Mat::min(int c)
{
    float value = std::numeric_limits<float>::max();

    for (int i = 0; i != w * h; ++i)
    {
        float current = data[c * h * w + i];
        if (current < value)
        {
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

int Mat::channelSize() const
{
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

//
// Math Matrix Code
// Rows,Cols
//

const float &Mat::m(const int row, const int col) const
{
    return data[row * w + col];
}

float &Mat::m(const int row, const int col)
{
    return data[row * w + col];
}

const float &Mat::operator()(const int row, const int col) const
{
    return m(row, col);
}

float &Mat::operator()(const int row, const int col)
{
    return m(row, col);
}

Mat &Mat::setIdentity()
{
    zero();

    for (int i = 0; i < h && i < w; ++i)
        m(i, i) = 1;

    return *this;
}

Mat Mat::augment() const
{
    Mat c(w * 2, h);
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            c.m(i, j) = m(i, j);

    for (int j = 0; j < h; ++j)
        c.m(j, j + w) = 1;

    return c;
}

Mat Mat::invert() const
{
    Mat none;

    // Matrix not square
    if (h != w)
        return none;

    Mat c = augment();

    for (int k = 0; k < c.h; ++k)
    {
        float p = 0.0f;
        int index = -1;
        for (int i = k; i < c.h; ++i)
        {
            float val = fabsf(c.m(i, k));
            if (val > p)
            {
                p = val;
                index = i;
            }
        }

        // "Can't do it, sorry!
        if (index == -1)
            return none;

        float swap;
        for (int j = 0; j != c.w; ++j)
        {
            swap = c.m(index, j);
            c.m(index, j) = c.m(k, j);
            c.m(k, j) = swap;
        }

        float val = c.m(k, k);
        c.m(k, k) = 1;
        for (int j = k + 1; j < c.w; ++j)
            c.m(k, j) /= val;

        for (int i = k + 1; i < c.h; ++i)
        {
            float s = -c.m(i, k);
            c.m(i, k) = 0;
            for (int j = k + 1; j < c.w; ++j)
                c.m(i, j) += s * c.m(k, j);
        }
    }

    for (int k = c.h - 1; k > 0; --k)
        for (int i = 0; i < k; ++i)
        {
            float s = -c.m(i, k);
            c.m(i, k) = 0;
            for (int j = k + 1; j < c.w; ++j)
                c.m(i, j) += s * c.m(k, j);
        }

    Mat inv(w, h);
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            inv.m(i, j) = c.m(i, j + w);

    return inv;
}

Mat Mat::makeIdentity(int rows, int cols)
{
    Mat mat(cols, rows);
    return mat.setIdentity();
}

Mat Mat::makeIdentity3x3()
{
    Mat H(3, 3);
    H.m(0, 0) = 1;
    H.m(1, 1) = 1;
    H.m(2, 2) = 1;
    return H;
}

Mat Mat::makeTranslation3x3(float dx, float dy)
{
    Mat H = makeIdentity3x3();
    H.m(0, 2) = dx;
    H.m(1, 2) = dy;
    return H;
}

void Mat::mmult(const Mat &a, const Mat &b, Mat &p)
{
    assert(a.w == b.h);
    p.reshape(a.w, a.h, 1);

    for (int i = 0; i < p.h; ++i)
        for (int j = 0; j < p.w; ++j)
            for (int k = 0; k < a.w; ++k)
                p.m(i, j) += a.m(i, k) * b.m(k, j);
}

Mat Mat::mmult(const Mat &a, const Mat &b)
{
    Mat p;
    mmult(a, b, p);
    return p;
}

void Mat::vmult(const Mat &a, const Mat &b, Mat &p)
{
    assert(b.size() == a.w);

    p.reshape(a.h, 1, 1);
    p.zero();

    for (int i = 0; i < a.h; ++i)
        for (int j = 0; j < a.w; ++j)
            p.data[i] += a(i, j) * b.data[j];
}

Mat Mat::vmult(const Mat &a, const Mat &b)
{
    Mat p;
    vmult(a, b, p);
    return p;
}

Mat Mat::llsSolve(Mat const& M, Mat const& b) {
    
    Mat none;
    Mat Mt = M.transpose();
    Mat MtM = Mat::mmult(Mt, M);
    Mat MtMinv = MtM.invert();

    if(MtMinv.size() == 0) 
        return none;

    Mat Mdag = Mat::mmult(MtMinv, Mt);
    Mat a = Mat::mmult(Mdag, b);
    
    return a;
}

} // namespace vs
