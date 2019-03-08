#include "vs.hpp"

namespace vs
{

void markSpot(Mat &im, Point const &p)
{
    int x = int(p.x);
    int y = int(p.y);
    for (int i = -9; i < 10; ++i)
    {
        im.setClamp(x + i, y, 0, 1);
        im.setClamp(x, y + i, 0, 1);
        im.setClamp(x + i, y, 1, 0);
        im.setClamp(x, y + i, 1, 0);
        im.setClamp(x + i, y, 2, 1);
        im.setClamp(x, y + i, 2, 1);
    }
}

void markCorners(Mat &im, Descriptors const &d)
{
    for (size_t i = 0; i < d.size(); ++i)
    {
        markSpot(im, d[i].p);
    }
}

void drawHarrisCorners(Mat &im, const float sigma, const float thresh, const int nms)
{
    Descriptors d = harrisCornerDetector(im, sigma, thresh, nms);
    markCorners(im, d);
}

Mat mergeSideBySide(const Mat &a, const Mat &b)
{
    Mat both(a.w + b.w, a.h > b.h ? a.h : b.h, a.c > b.c ? a.c : b.c);

    for(int k = 0; k < a.c; ++k)
        for(int j = 0; j < a.h; ++j)
            for(int i = 0; i < a.w; ++i)
                both.set(i, j, k, a.get(i, j, k));

    for(int k = 0; k < b.c; ++k)
        for(int j = 0; j < b.h; ++j)
            for(int i = 0; i < b.w; ++i)
                both.set(i+a.w, j, k, b.get(i, j, k));

    return both;
}

Mat drawMatches(const Mat &a, const Mat &b, const Matches &matches, int inliers)
{
    Mat both = mergeSideBySide(a, b);

    for(size_t i = 0; i < matches.size(); ++i){
        int bx = int(matches[i].p.x);
        int ex = int(matches[i].q.x);
        int by = int(matches[i].p.y);
        int ey = int(matches[i].q.y);

        for(int j = bx; j < ex + a.w; ++j){
            //int r = (float)(j-bx)/(ex+a.w - bx)*(ey - by) + by;
            int r = int(float(j-bx)/(ex+a.w - bx)*(ey - by) + by);
            both.set(j, r, 0, int(i) < inliers ? 0.0f : 1.0f);
            both.set(j, r, 1, int(i) < inliers ? 1.0f : 0.0f);
            both.set(j, r, 2, 0.0f);
        }
    }
    return both;
}

Mat drawInliers(const Mat &a, const Mat &b, const Mat &H, Matches &m, float thresh)
{
    int inliers = modelInliers(H, m, thresh);
    return drawMatches(a, b, m, inliers);
}

Mat drawMatches(Mat &a, Mat &b, float sigma, float thresh, int nms)
{
    Descriptors ad = harrisCornerDetector(a, sigma, thresh, nms);
    Descriptors bd = harrisCornerDetector(b, sigma, thresh, nms);
    Matches m = matchDescriptors(ad, bd);
    markCorners(a, ad);
    markCorners(b, bd);
    return drawMatches(a, b, m, 0);
}

} // namespace vs