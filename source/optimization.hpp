#pragma once

#include "vs.hpp"

// http://dlib.net/dlib/optimization/max_cost_assignment.h.html

namespace vs
{
/*
inline void computeSlack(
        const size_t x,
        std::vector<long>& slack,
        std::vector<long>& slackx,
        const Matl& cost,
        const std::vector<long>& lx,
const std::vector<long>& ly
        )
{
    for (size_t y = 0; y < size_t(cost.w); ++y)
    {
        if (lx[x] + ly[y] - cost(x,y) < slack[y])
        {
            slack[y] = lx[x] + ly[y] - cost(x,y);
            slackx[y] = x;
        }
    }
}
*/
std::vector<long> maxCostAssignment(const Matl& cost_)
{
    return std::vector<long>();
}
} // namespace vs
