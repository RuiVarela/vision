#pragma once

#include "vs.hpp"

namespace vs
{

// A 2d point.
// float x, y: the coordinates of the point.
struct Point
{
    Point() :x(0.0f), y(0.0f) {}
    Point(float px, float py) :x(px), y(py) {}

    float x;
    float y;

    // Calculate L2 distance between two points.
    // Minkowski distance between two points of order 2
    // https://en.wikipedia.org/wiki/Minkowski_distance
    // point p, q: points.
    // returns: L2 distance between them.
    static float distance(Point const& p, Point const& q);
};

Mat loadImage(std::string path, int channels = 0);
bool saveImage(std::string path, Mat const &im);

void rgb2gray(Mat const& src, Mat &dst);
Mat rgb2gray(Mat const& src);

void rgb2bgr(Mat const& src, Mat &dst);
Mat rgb2bgr(Mat const& src);
void rgb2bgrInplace(Mat &inplace);

void rgb2hsv(Mat const& src, Mat &dst);
Mat rgb2hsv(Mat const& src);
void rgb2hsvInplace(Mat &inplace);

void hsv2rgb(Mat const& src, Mat &dst);
Mat hsv2rgb(Mat const& src);
void hsv2rgbInplace(Mat &inplace);


enum ResizeMode
{
    NearestNeighbor,
    Bilinear
};
float interpolateNN(Mat const& im, float x, float y, int c);
float interpolateBL(Mat const& im, float x, float y, int c);
void resize(Mat const& src, Mat &dst, int nw, int nh, ResizeMode const mode = Bilinear);
Mat resize(Mat const& src, int nw, int nh, ResizeMode const mode = Bilinear);


vs::Mat cylindricalProject(vs::Mat const &im, float f);

// http://dlib.net/imaging.html#extract_image_4points
// The 4 points in pts define a convex quadrilateral and this function extracts
// that part of the input image img and stores it into dst.  Therefore, each
// corner of the quadrilateral is associated to a corner of dst and bilinear
// interpolation and a projective mapping is used to transform the pixels in the
// quadrilateral into dst.  To determine which corners of the quadrilateral map
// to which corners of dst we fit the tightest possible rectangle to the
// quadrilateral and map its vertices to their nearest rectangle corners.  These
// corners are then trivially mapped to dst (i.e.  upper left corner to upper
// left corner, upper right corner to upper right corner, etc.).
void extractImage4points(Mat const& im, Mat &dst, const std::array<Point,4>& points);
} // namespace vs
