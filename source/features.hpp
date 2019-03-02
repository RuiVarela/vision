#pragma once

#include "vs.hpp"

namespace vs
{

// A 2d point.
// float x, y: the coordinates of the point.
struct Point
{
    float x = 0.0f;
    float y = 0.0f;
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
  private:
    std::shared_ptr<float> shared_data;
};
using Descriptors = std::vector<Descriptor>;

// Marks the spot of a point in an image.
// image im: image to mark.
// point p: spot to mark in the image.
void markSpot(Mat &im, Point const& p);

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptors d: corners in the image.
void markCorners(Mat &im, Descriptors const& d);

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

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detectAndDrawHarrisCorners(Mat &im, float const sigma, float const thresh, int const nms);

} // namespace vs