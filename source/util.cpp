#include "vs.hpp"

namespace vs
{

std::string toNativeSeparators(const std::string &path)
{
    std::string output = path;

#ifdef __linux__

#elif _WIN32
    std::replace(output.begin(), output.end(), '\\', '/');
#endif

    return output;
}

bool sameMat(const Mat &a, const Mat &b)
{
    float const epsilon = 0.005f;

    if (a.w != b.w || a.h != b.h || a.c != b.c)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.w, b.h, b.c, a.w, a.h, a.c);
        return false;
    }

    for (int k = 0; k < a.c; ++k)
    {
        for (int y = 0; y < a.h; ++y)
        {
            for (int x = 0; x < a.w; ++x)
            {
                int index = a.w * a.h * k + a.w * y + x;
                if (!equivalent(a.data[index], b.data[index], epsilon))
                {
                    printf("(%d %d %d) %f %f\n", x, y, k, a.data[index], b.data[index]);
                    return false;
                }
            }
        }
    }

    return true;
}


std::ostream &print(Mat const& m, std::ostream &out, int max_cols, int max_rows)
{
    out << "Header " << m.w << "x" << m.h << "x" << m.c << std::endl;
    out << std::fixed << std::setprecision(3);

    if (max_cols < 0)
    {
        max_cols = m.w;
    }

    if (max_rows < 0)
    {
        max_rows = m.h;
    }

    if (max_rows > 0 && max_rows > 0)
    {
        for (int i = 0; i < m.c; ++i)
        {
            out << "channel " << i << std::endl;

            for (int j = 0; j < m.h; ++j)
            {
                if (j > max_rows)
                {
                    out << "... (" << m.h - j << ")" << std::endl;
                    break;
                }

                for (int k = 0; k < m.w; ++k)
                {
                    if (k > max_cols)
                    {
                        out << " ... (" << m.w - k << ")";
                        break;
                    }

                    if (k > 0)
                    {
                        out << ", ";
                    }

                    out << std::fixed << m.data[i * m.h * m.w + j * m.w + k];
                }
                out << std::endl;
            }
        }
    }
    return out;
}


} // namespace vs
