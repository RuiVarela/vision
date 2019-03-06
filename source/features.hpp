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
    // point p, q: points.
    // returns: L2 distance between them.
    static float distance(Point const& p, Point const& q);
};

// A descriptor for a point in an image.
// point p: x,y coordinates of the image pixel.
// int n: the number of floating point values in the descriptor.
// float *data: the descriptor for the pixel.
struct Descriptor
{
    Point p;
    int n = 0;
    float *data = nullptr;

    void reshape(int size);

    // Calculates L1 distance between to floating point arrays.
    // float *a, *b: arrays to compare.
    // int n: number of values in each array.
    // returns: l1 distance between arrays (sum of absolute differences).
    static float l1Distance(float *a, float *b, int n);

  private:
    std::shared_ptr<float> shared_data;
};
using Descriptors = std::vector<Descriptor>;

// A match between two points in an image.
// point p, q: x,y coordinates of the two matching pixels.
// int ai, bi: indexes in the descriptor array. For eliminating duplicates.
// float distance: the distance between the descriptors for the points.
struct Match {
    Point p;
    Point q;
    int ai = -1;
    int bi = -1;
    float distance;
};
using Matches = std::vector<Match>;

// Comparator for matches
// const void *a, *b: pointers to the matches to compare.
// returns: result of comparison, 0 if same, 1 if a > b, -1 if a < b.
int matchCompare(Match& a, Match& b);

// Finds best matches between descriptors of two images.
// descriptor *a, *b: array of descriptors for pixels in two images.
// int an, bn: number of descriptors in arrays a and b.
// int *mn: pointer to number of matches found, to be filled in by function.
// returns: best matches found. each descriptor in a should match with at most
//          one other descriptor in b.
Matches matchDescriptors(Descriptors const& a, Descriptors const& b);

// Count number of inliers in a set of matches. Should also bring inliers to the front of the array.
// matrix H: homography between coordinate systems.
// match *m: matches to compute inlier/outlier.
// float thresh: threshold to be an inlier.
// returns: number of inliers whose projected point falls within thresh of
//          their match in the other image. Should also rearrange matches
//          so that the inliers are first in the array. For drawing.
int modelInliers(Mat const& H, Matches& m, float thresh);

// Randomly shuffle matches for RANSAC.
// Fisher-Yate
// match *m: matches to shuffle in place.
void randomizeMatches(Matches& m);

// Computes homography between two images given matching pixels.
// match *matches: matching points between images.
// returns: matrix representing homography H that maps image a to image b.
Mat computeHomography(Matches const& matches);

// Perform RANdom SAmple Consensus to calculate homography for noisy matches.
// match *m: set of matches.
// float thresh: inlier/outlier distance threshold.
// int k: number of iterations to run.
// int cutoff: inlier cutoff to exit early.
// returns: matrix representing most common homography between matches.
Mat RANSAC(Matches& m, float thresh, int k, int cutoff);

// Apply a projective transformation to a point.
// matrix H: homography to project point.
// point p: point to project.
// returns: point projected using the homography.
Point projectPoint(Mat const& H, Point const& p);

// Create a feature descriptor for an index in an image.
// very simple descriptior : its just a patch of neighbors pixels with the central one subtracted
// to compensate some for exposure/lighting changes.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
Descriptor describeIndex(Mat const&im, int i);

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// dst - output: image with only local-maxima responses within w pixels.
void nonMaxSupression(Mat const& im, Mat& dst, int w);

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// image s: output - structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2, third channel is IxIy.
void harrisStructureMatrix(Mat const& im, Mat& S,float sigma);

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// image r: output - a response map of cornerness calculations.
void harrisCornernessResponse(Mat const& S, Mat& R);

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
Descriptors harrisCornerDetector(Mat const& im, float sigma, float thresh, int nms);

} // namespace vs
