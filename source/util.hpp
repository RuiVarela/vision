#pragma once

namespace vs
{
    std::string toNativeSeparators(std::string const& path);


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

} // namespace cv
