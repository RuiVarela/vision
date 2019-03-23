#pragma once

#include "vs.hpp"

namespace vs
{

// this code was copied from dlib
// http://dlib.net/dlib/optimization/max_cost_assignment.h.html
using Assignment = std::vector<long>;
using CostMatrix = Matl;

// calculates the actual cost for the input assigment
long assignmentCost(const CostMatrix &cost, const Assignment &assignment);

// Hungarian algorithm for max cost assigment
Assignment assignmentMaxCost(const CostMatrix &cost);

} // namespace vs
