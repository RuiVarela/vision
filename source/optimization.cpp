#include "vs.hpp"

namespace vs
{

long assignmentCost(const CostMatrix &cost, const Assignment &assignment)
{
    assert(cost.w == cost.h);
    assert(assignment.size() > 0);

    long sum = 0;
    for (size_t i = 0; i < assignment.size(); ++i)
        sum += cost(int(i), int(assignment[i]));

    return sum;
}

inline void compute_slack(
    const size_t x,
    std::vector<long> &slack,
    std::vector<long> &slackx,
    const CostMatrix &cost,
    const std::vector<long> &lx, const std::vector<long> &ly)
{
    for (size_t y = 0; y < cost.w; ++y)
        if (lx[x] + ly[y] - cost(x, y) < slack[y])
        {
            slack[y] = lx[x] + ly[y] - cost(x, y);
            slackx[y] = x;
        }
}

Assignment assignmentMaxCost(const CostMatrix &cost)
{
    // This algorithm only works if the elements of the cost matrix can be reliably
    // compared using operator==. However, comparing for equality with floating point
    // numbers is not a stable operation. So you need to use an integer cost matrix.
    assert(std::numeric_limits<CostMatrix::Type>::is_integer);
    assert(cost.w == cost.h);

    // I based the implementation of this algorithm on the description of the
    // Hungarian algorithm on the following websites:
    //     http://www.math.uwo.ca/~mdawes/courses/344/kuhn-munkres.pdf
    //     http://www.topcoder.com/tc?module=Static&d1=tutorials&d2=hungarianAlgorithm

    // Note that this is the fast O(n^3) version of the algorithm.

    if (cost.size() == 0)
        return Assignment();

    std::vector<long> lx, ly;
    std::vector<long> xy;
    std::vector<long> yx;
    std::vector<char> S, T;
    std::vector<long> slack;
    std::vector<long> slackx;
    std::vector<long> aug_path;

    size_t cost_nc = cost.w;

    // Initially, nothing is matched.
    xy.assign(cost_nc, -1);
    yx.assign(cost_nc, -1);

    // We maintain the following invariant:
    //     Vertex x is matched to vertex xy[x] and
    //     vertex y is matched to vertex yx[y].

    //     A value of -1 means a vertex isn't matched to anything.  Moreover,
    //     x corresponds to rows of the cost matrix and y corresponds to the
    //     columns of the cost matrix.  So we are matching X to Y.

    // Create an initial feasible labeling.  Moreover, in the following
    // code we will always have:
    //     for all valid x and y:  lx[x] + ly[y] >= cost(x,y)
    // lx.resize(cost_nc);
    // ly.assign(cost_nc, 0);
    // for (long x = 0; x < cost.nr(); ++x)
    //     lx[x] = max(rowm(cost, x));

    /*
        // Now grow the match set by picking edges from the equality subgraph until
        // we have a complete matching.
        for (long match_size = 0; match_size < cost.nc(); ++match_size)
        {
            std::deque<long> q;

            // Empty out the S and T sets
            S.assign(cost.nc(), false);
            T.assign(cost.nc(), false);

            // clear out old slack values
            slack.assign(cost.nc(), std::numeric_limits<type>::max());
            slackx.resize(cost.nc());
 
                slack and slackx are maintained such that we always
                have the following (once they get initialized by compute_slack() below):
                    - for all y:
                        - let x == slackx[y]
                        - slack[y] == lx[x] + ly[y] - cost(x,y)


            aug_path.assign(cost.nc(), -1);

            for (long x = 0; x < cost.nc(); ++x)
            {
                // If x is not matched to anything
                if (xy[x] == -1)
                {
                    q.push_back(x);
                    S[x] = true;

                    compute_slack(x, slack, slackx, cost, lx, ly);
                    break;
                }
            }


            long x_start = 0;
            long y_start = 0;

            // Find an augmenting path.  
            bool found_augmenting_path = false;
            while (!found_augmenting_path)
            {
                while (q.size() > 0 && !found_augmenting_path)
                {
                    const long x = q.front();
                    q.pop_front();
                    for (long y = 0; y < cost.nc(); ++y)
                    {
                        if (cost(x,y) == lx[x] + ly[y] && !T[y])
                        {
                            // if vertex y isn't matched with anything
                            if (yx[y] == -1) 
                            {
                                y_start = y;
                                x_start = x;
                                found_augmenting_path = true;
                                break;
                            }

                            T[y] = true;
                            q.push_back(yx[y]);

                            aug_path[yx[y]] = x;
                            S[yx[y]] = true;
                            compute_slack(yx[y], slack, slackx, cost, lx, ly);
                        }
                    }
                }

                if (found_augmenting_path)
                    break;


                // Since we didn't find an augmenting path we need to improve the 
                // feasible labeling stored in lx and ly.  We also need to keep the
                // slack updated accordingly.
                type delta = std::numeric_limits<type>::max();
                for (unsigned long i = 0; i < T.size(); ++i)
                {
                    if (!T[i])
                        delta = std::min(delta, slack[i]);
                }
                for (unsigned long i = 0; i < T.size(); ++i)
                {
                    if (S[i])
                        lx[i] -= delta;

                    if (T[i])
                        ly[i] += delta;
                    else
                        slack[i] -= delta;
                }



                q.clear();
                for (long y = 0; y < cost.nc(); ++y)
                {
                    if (!T[y] && slack[y] == 0)
                    {
                        // if vertex y isn't matched with anything
                        if (yx[y] == -1)
                        {
                            x_start = slackx[y];
                            y_start = y;
                            found_augmenting_path = true;
                            break;
                        }
                        else
                        {
                            T[y] = true;
                            if (!S[yx[y]])
                            {
                                q.push_back(yx[y]);

                                aug_path[yx[y]] = slackx[y];
                                S[yx[y]] = true;
                                compute_slack(yx[y], slack, slackx, cost, lx, ly);
                            }
                        }
                    }
                }
            } // end while (!found_augmenting_path)

            // Flip the edges along the augmenting path.  This means we will add one more
            // item to our matching.
            for (long cx = x_start, cy = y_start, ty; 
                 cx != -1; 
                 cx = aug_path[cx], cy = ty)
            {
                ty = xy[cx];
                yx[cy] = cx;
                xy[cx] = cy;
            }

        }


        return xy;
        */
}

} // namespace vs
