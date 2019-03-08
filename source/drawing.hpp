#pragma once

#include "vs.hpp"

namespace vs
{
// Marks the spot of a point in an image.
// image im: image to mark.
// point p: spot to mark in the image.
void markSpot(Mat &im, Point const& p);

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptors d: corners in the image.
void markCorners(Mat &im, Descriptors const& d);

// Place two images side by side on canvas, for drawing matching pixels.
// image a, b: images to place.
// returns: image with both a and b side-by-side.
Mat mergeSideBySide(Mat const& a, Mat const& b);

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void drawHarrisCorners(Mat &im, float const sigma, float const thresh, int const nms);

// Draws lines between matching pixels in two images.
// image a, b: two images that have matches.
// match *matches: array of matches between a and b.
// int inliers: number of inliers at beginning of matches, drawn in green.
// returns: image with matches drawn between a and b on same canvas.
Mat drawMatches(Mat const& a, Mat const& b, Matches const& matches, int inliers);


// Draw the matches with inliers in green between two images.
// image a, b: two images to match.
// matches *
Mat drawInliers(Mat const& a, Mat const& b, Matd const& H, Matches& m, float thresh);

// Find corners, match them, and draw them between two images.
// image a, b: images to match.
// float sigma: gaussian for harris corner detector. Typical: 2
// float thresh: threshold for corner/no corner. Typical: 1-5
// int nms: window to perform nms on. Typical: 3
Mat drawMatches(Mat& a, Mat& b, float sigma, float thresh, int nms);



} // namespace cv
