#pragma once

#include "vs.hpp"

namespace vs
{

template<typename T>
class MatT
{
public:
    using Type = T;

    MatT();
    explicit MatT(int w, int h = 1, int c = 1);
    explicit MatT(int w, int h, int c, T* ext); // external memory pointer

    void reshape(int w, int h, int c);
    int size() const;
    int channelSize() const;
    MatT clone() const;

    // views
    // these are virtual view on a Mat data, they will point to the parent memory
    // thus they don't allocate new memory
    MatT channelView(int c, int count = 1);

    MatT &zero();
    MatT &fill(int c, T v); // fills a channel with v value
    MatT &fill(T v); // fills all channels with v value
    MatT &fill(MatT const& src, int src_c, int dst_c); // fills a channel with values from another mat

    // channel values get/set
    T get(int x, int y = 0, int c = 0) const;
    T getClamp(int x, int y, int c) const;
    T getZero(int x, int y, int c) const;
    MatT &set(int x, int y, int c, T v);
    MatT &setClamp(int x, int y, int c, T v);

    // clamp all values between [min, max]
    MatT &clamp(int c, T min, T max);
    MatT &clamp(T min, T max);
    MatT &clamp(); // 0.0f, 1.0f

    // Add a scalar value
    MatT &add(int c, T v);
    MatT &add(T v);
    // Add a mat
    MatT &add(MatT const &v);
    static MatT add(MatT const &a, MatT const &b);

    // Subtract a mat
    MatT &sub(MatT const &v);
    static MatT sub(MatT const &a, MatT const &b);

    // Multiply by a scalar
    MatT &mult(int c, T v);
    MatT &mult(T v);

    // transpose
    MatT transpose() const;

    T sum(int c); // sums all values in a channel
    T max(int c); // max value in a channel
    T min(int c); // min value in a channel
    void minNmax(int c, T& minv, T& maxv); // min and max value in a channel

    // normalizes a channels using l1norm
    MatT &l1Normalize(int c);
    MatT &l1Normalize();

    MatT &featureNormalize(int c);
    MatT &featureNormalize();


    //
    // Math Matrix Code
    // Rows,Cols
    //
    const T &operator()(int const row, int const col) const;
    T &operator()(int const row, int const col);


    MatT &setIdentity();

    MatT augment() const;
    MatT invert() const;

    static MatT makeIdentity(int rows, int cols);
    static MatT makeIdentity3x3();
    static MatT makeTranslation3x3(T dx, T dy);

    // Matrix-Matrix multiplication
    // p = a * b
    static void mmult(const MatT &a, const MatT &b, MatT& p);
    static MatT mmult(const MatT &a, const MatT &b);

    // Matrix-Vector multiplication
    // vp = ma * vb
    static void vmult(const MatT &a, const MatT &b, MatT& p);
    static MatT vmult(const MatT &a, const MatT &b);

    // linear least squares solver
    // https://textbooks.math.gatech.edu/ila/least-squares.html
    static MatT llsSolve(MatT const& M, MatT const& b);


    int w;    // width
    int h;    // height
    int c;    // channels;
    T *data;
private:
    std::shared_ptr<T> shared_data;
};



using Mat = MatT<float>;
using Matd = MatT<double>;

//
// Implementation
//

template<typename T>
inline MatT<T>::MatT()
    : w(0), h(0), c(0), data(nullptr)
{
}

template<typename T>
inline MatT<T>::MatT(int w, int h, int c)
    : MatT()
{
    reshape(w, h, c);
}

template<typename T>
inline MatT<T>::MatT(int w, int h, int c, T *ext)
    : MatT()
{
    assert(ext != nullptr);

    this->w = w;
    this->h = h;
    this->c = c;
    this->data = ext;
}

template<typename T>
inline MatT<T> MatT<T>::clone() const
{
    MatT<T> output(w, h, c);
    memcpy(output.data, data, size_t(size()) * sizeof(T));
    return output;
}

template<typename T>
inline MatT<T> MatT<T>::channelView(int c, int count)
{
    assert(this->c > (c + count - 1));

    MatT<T> output(w, h, count, data + c * channelSize());
    // share the data across mat objets
    output.shared_data = shared_data;

    return output;
}

template<typename T>
inline MatT<T> &MatT<T>::zero()
{
    memset(data, 0, size_t(size()) * sizeof(T));
    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::fill(int c, T v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = v;

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::fill(T v)
{
    for (int k = 0; k != c; ++k)
        fill(k, v);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::fill(MatT<T> const &src, int src_c, int dst_c)
{
    assert(src.w == w && src.h == h && src.c > src_c && c > dst_c);

    for (int i = 0; i != w * h; ++i)
        data[dst_c * w * h + i] = src.data[src_c * w * h + i];

    return *this;
}

template<typename T>
inline T MatT<T>::get(int x, int y, int c) const
{
    assert(x >= 0 && x < this->w && y >= 0 && y < this->h && c >= 0 && c < this->c);
    return data[c * h * w + y * w + x];
}

template<typename T>
inline T MatT<T>::getClamp(int x, int y, int c) const
{
    x = vs::clampTo(x, 0, this->w - 1);
    y = vs::clampTo(y, 0, this->h - 1);
    c = vs::clampTo(c, 0, this->c - 1);
    return data[c * h * w + y * w + x];
}

template<typename T>
inline T MatT<T>::getZero(int x, int y, int c) const
{
    if (x < 0 || x >= this->w || y < 0 || y >= this->h || c < 0 || c >= this->c)
        return 0;

    return data[c * h * w + y * w + x];
}

template<typename T>
inline MatT<T> &MatT<T>::set(int x, int y, int c, T v)
{
    assert(x >= 0 && y >= 0 && c >= 0);
    assert(x < w && y < h && c < this->c);

    data[c * h * w + y * w + x] = v;

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::setClamp(int x, int y, int c, T v)
{
    x = vs::clampTo(x, 0, this->w - 1);
    y = vs::clampTo(y, 0, this->h - 1);
    c = vs::clampTo(c, 0, this->c - 1);
    data[c * h * w + y * w + x] = v;
    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::add(int c, T v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] += v;

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::add(T v)
{
    for (int k = 0; k != c; ++k)
        add(k, v);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::add(const MatT<T> &v)
{
    assert(c == v.c && w == v.w && h == v.h);

    for (int i = 0; i != c * w * h; ++i)
        data[i] += v.data[i];

    return *this;
}

template<typename T>
inline MatT<T> MatT<T>::add(const MatT<T> &a, const MatT<T> &b)
{
    MatT<T> output = a.clone();
    output.add(b);
    return output;
}

template<typename T>
inline MatT<T> &MatT<T>::sub(const MatT<T> &v)
{
    assert(c == v.c && w == v.w && h == v.h);

    for (int i = 0; i != c * w * h; ++i)
        data[i] -= v.data[i];

    return *this;
}

template<typename T>
inline MatT<T> MatT<T>::sub(const MatT<T> &a, const MatT<T> &b)
{
    MatT<T> output = a.clone();
    output.sub(b);
    return output;
}

template<typename T>
inline MatT<T> &MatT<T>::mult(int c, T v)
{
    assert(c >= 0 && c < this->c);

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] *= v;

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::mult(T v)
{
    for (int k = 0; k != c; ++k)
        mult(k, v);

    return *this;
}

template<typename T>
inline MatT<T> MatT<T>::transpose() const
{
    MatT<T> t(h, w, c);

    for (int k = 0; k != t.c; ++k)
        for (int y = 0; y != t.h; ++y)
            for (int x = 0; x != t.w; ++x)
                t.set(x, y, k, get(y, x, k));

    return t;
}

template<typename T>
inline T MatT<T>::sum(int c)
{
    T value = 0.0;

    for (int i = 0; i != w * h; ++i)
        value += data[c * h * w + i];

    return value;
}

template<typename T>
inline T MatT<T>::max(int c)
{
    T value = std::numeric_limits<T>::min();

    for (int i = 0; i != w * h; ++i)
    {
        T current = data[c * h * w + i];
        if (current > value)
        {
            value = current;
        }
    }

    return value;
}

template<typename T>
inline T MatT<T>::min(int c)
{
    T value = std::numeric_limits<T>::max();

    for (int i = 0; i != w * h; ++i)
    {
        T current = data[c * h * w + i];
        if (current < value)
        {
            value = current;
        }
    }

    return value;
}

template<typename T>
inline void MatT<T>::minNmax(int c, T &minv, T &maxv)
{
    minv = std::numeric_limits<T>::max();
    maxv = std::numeric_limits<T>::min();

    for (int i = 0; i != w * h; ++i)
    {
        T current = data[c * h * w + i];
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

template<typename T>
inline MatT<T> &MatT<T>::featureNormalize(int c)
{
    T max_v;
    T min_v;
    minNmax(c, min_v, max_v);

    T delta = max_v - min_v;

    if (equivalent(delta, T(0.0)))
        return zero();

    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = (data[c * h * w + i] - min_v) / delta;

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::featureNormalize()
{
    for (int k = 0; k != c; ++k)
        featureNormalize(k);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::l1Normalize(int c)
{
    T channel_sum = sum(c);
    T value = 1.0f / channel_sum;
    return mult(c, value);
}

template<typename T>
inline MatT<T> &MatT<T>::l1Normalize()
{
    for (int k = 0; k != c; ++k)
        l1Normalize(k);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::clamp(int c, T min, T max)
{
    for (int i = 0; i != w * h; ++i)
        data[c * h * w + i] = vs::clampTo(data[c * h * w + i], min, max);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::clamp(T min, T max)
{
    for (int k = 0; k != c; ++k)
        clamp(k, min, max);

    return *this;
}

template<typename T>
inline MatT<T> &MatT<T>::clamp()
{
    return clamp(0, 1);
}

template<typename T>
inline int MatT<T>::size() const
{
    return w * h * c;
}

template<typename T>
inline int MatT<T>::channelSize() const
{
    return w * h;
}

template<typename T>
inline void MatT<T>::reshape(int w, int h, int c)
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

        data = static_cast<T *>(calloc(size_t(size()), sizeof(T)));
        if (data)
        {
            shared_data = std::shared_ptr<T>(data, free);
        }
    }
}

//
// Math Matrix Code
// Rows,Cols
//

template<typename T>
inline const T &MatT<T>::operator()(const int row, const int col) const
{
    return data[row * w + col];
}

template<typename T>
inline T &MatT<T>::operator()(const int row, const int col)
{
    return data[row * w + col];
}

template<typename T>
inline MatT<T> &MatT<T>::setIdentity()
{
    zero();

    for (int i = 0; i < h && i < w; ++i)
        (*this)(i, i) = 1;

    return *this;
}

template<typename T>
inline MatT<T> MatT<T>::augment() const
{
    MatT<T> c(w * 2, h);
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            c(i, j) = (*this)(i, j);

    for (int j = 0; j < h; ++j)
        c(j, j + w) = 1;

    return c;
}

template<typename T>
inline MatT<T> MatT<T>::invert() const
{
    MatT<T> none;

    // Matrix not square
    if (h != w)
        return none;

    MatT<T> c = augment();

    for (int k = 0; k < c.h; ++k)
    {
        T p = 0;
        int index = -1;
        for (int i = k; i < c.h; ++i)
        {
            T val = fabsf(c(i, k));
            if (val > p)
            {
                p = val;
                index = i;
            }
        }

        // "Can't do it, sorry!
        if (index == -1)
            return none;

        T swap;
        for (int j = 0; j != c.w; ++j)
        {
            swap = c(index, j);
            c(index, j) = c(k, j);
            c(k, j) = swap;
        }

        T val = c(k, k);
        c(k, k) = 1;
        for (int j = k + 1; j < c.w; ++j)
            c(k, j) /= val;

        for (int i = k + 1; i < c.h; ++i)
        {
            T s = -c(i, k);
            c(i, k) = 0;
            for (int j = k + 1; j < c.w; ++j)
                c(i, j) += s * c(k, j);
        }
    }

    for (int k = c.h - 1; k > 0; --k)
        for (int i = 0; i < k; ++i)
        {
            T s = -c(i, k);
            c(i, k) = 0;
            for (int j = k + 1; j < c.w; ++j)
                c(i, j) += s * c(k, j);
        }

    MatT<T> inv(w, h);
    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            inv(i, j) = c(i, j + w);

    return inv;
}

template<typename T>
inline MatT<T> MatT<T>::makeIdentity(int rows, int cols)
{
    MatT<T> mat(cols, rows);
    return mat.setIdentity();
}

template<typename T>
inline MatT<T> MatT<T>::makeIdentity3x3()
{
    MatT<T> H(3, 3);
    H(0, 0) = 1;
    H(1, 1) = 1;
    H(2, 2) = 1;
    return H;
}

template<typename T>
inline MatT<T> MatT<T>::makeTranslation3x3(T dx, T dy)
{
    MatT<T> H = makeIdentity3x3();
    H(0, 2) = dx;
    H(1, 2) = dy;
    return H;
}

template<typename T>
inline void MatT<T>::mmult(const MatT<T> &a, const MatT<T> &b, MatT<T> &p)
{
    assert(a.w == b.h);
    p.reshape(b.w, a.h, 1);
    p.zero();

    for (int i = 0; i < p.h; ++i)
        for (int j = 0; j < p.w; ++j)
            for (int k = 0; k < a.w; ++k)
                p(i, j) += a(i, k) * b(k, j);
}

template<typename T>
inline MatT<T> MatT<T>::mmult(const MatT<T> &a, const MatT<T> &b)
{
    MatT<T> p;
    mmult(a, b, p);
    return p;
}

template<typename T>
inline void MatT<T>::vmult(const MatT<T> &a, const MatT<T> &b, MatT<T> &p)
{
    assert(b.size() == a.w);

    p.reshape(1, a.h, 1);
    p.zero();

    for (int i = 0; i < a.h; ++i)
        for (int j = 0; j < a.w; ++j)
            p.data[i] += a(i, j) * b.data[j];
}

template<typename T>
inline MatT<T> MatT<T>::vmult(const MatT<T> &a, const MatT<T> &b)
{
    MatT<T> p;
    vmult(a, b, p);
    return p;
}

template<typename T>
inline MatT<T> MatT<T>::llsSolve(MatT<T> const& M, MatT<T> const& b) {

    MatT<T> none;
    MatT<T> Mt = M.transpose();
    MatT<T> MtM = MatT<T>::mmult(Mt, M);
    MatT<T> MtMinv = MtM.invert();

    if(MtMinv.size() == 0)
        return none;

    MatT<T> Mdag = MatT<T>::mmult(MtMinv, Mt);
    MatT<T> a = MatT<T>::mmult(Mdag, b);

    return a;
}

} // namespace vs
