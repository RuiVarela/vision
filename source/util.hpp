#pragma once

#include "vs.hpp"

namespace vs
{
std::string toNativeSeparators(std::string const& path);

bool findArg(int argc, char* argv[], std::string arg);
int findArgInt(int argc, char **argv, std::string arg, int def);
float findArgFloat(int argc, char **argv, std::string arg, float def);
std::string findArgStr(int argc, char **argv, std::string arg, std::string def);

bool sameMat(Mat const &a, Mat const &b);
bool sameChannel(Mat const &a, Mat const &b, int const ac, int const bc);

template<typename T>
std::ostream &print(MatT<T> const& m, std::ostream &out = std::cout, int max_cols = -1, int max_rows = -1);


template<typename T>
inline T minimum(T const a, T const b) { return (a < b) ? a: b; }

template<typename T>
inline T minimum(T const a, T const b, T const c) { return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c); }

template<typename T>
inline T maximum(T const a, T const b) { return (a > b) ? a : b; }

template<typename T>
inline T maximum(T const a, T const b, T const c) { return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c); }

template<typename T>
inline T absolute(T const value) { return ((value < T(0.0)) ? -value : value); }

template<typename T>
inline bool equivalent(T const left, T const right, T const epsilon = 1e-6) { T delta = right - left; return (delta < T(0.0)) ? (delta >= -epsilon) : (delta <= epsilon); }

template<typename T>
inline T sign(T const value) { return ((value < T(0.0)) ? T(-1.0) : ((value == T(0.0)) ? T(0.0) : T(1.0)) ); }

template<typename T>
inline T square(T const value) { return value * value; }

template<typename T>
inline T clampNear(T const value, T const target, T const epsilon = 1e-6) { return (equivalent(value, target, epsilon) ? target : value); }

template<typename T>
inline T clampTo(T const value, T const minimum, T const maximum) { return ((value < minimum) ? minimum : ((value > maximum) ? maximum : value) ); }

template<typename T>
inline T clampAbove(T const value, T const minimum) { return ((value < minimum) ? minimum : value); }

template<typename T>
inline T clampBelow(T const value, T const maximum) { return ((value > maximum) ? maximum : value); }

template<typename T>
inline T round(T const value) { return ( (value >= T(0.0)) ? floor(value + T(0.5)) : ceil(value - T(0.5)) ); }


//
// Implementation
//

template<typename T>
inline std::ostream &print(MatT<T> const& m, std::ostream &out, int max_cols, int max_rows)
{
    out << "Header " << m.w << "x" << m.h << "x" << m.c << std::endl;
    out << std::fixed << std::setprecision(3);

    if (max_cols < 0)
        max_cols = m.w;

    if (max_rows < 0)
        max_rows = m.h;

    if (max_rows > 0 && max_rows > 0)
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
                        out << ", ";

                    out << std::fixed << m.data[i * m.h * m.w + j * m.w + k];
                }
                out << std::endl;
            }
        }

    return out;
}

} // namespace cv
