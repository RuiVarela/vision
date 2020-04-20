# vision
A playground with no optimizations, and "almost" no dependencies
It is not intended to be fast. it is intended to be clear.

## Features
- Planar image represention using floats
- Basic Mat structure with simple usage
- Nearest Neighbor and Bilinear interpolation resize
- Color Conversion (rgb <-> hsv)
- Convolutions
- Filters (Gaussian, sobel, etc)
- Harris Corner detector
- Shi-Tomasi Corner detector
- Homography calculation
- RANSAC fitting example for noisy matched features
- Lukas Kanade optical flow calculation
- Canny Edge Detector
- Max Cost Assigment
- Image rectangle extraction and warping
- Thresholding (Binary, BinaryInverted, Truncate, ToZero, ToZeroInverted) with otsu

# Sources
This started as a fun exercise to solve Joseph Redmon CSE 455 homeworks. so at its core the base structure should resemble his assigments
- [Vision Homework 1](https://github.com/pjreddie/vision-hw0)
- [Vision Homework 2](https://github.com/pjreddie/vision-hw1)  
- [Vision Homework 3](https://github.com/pjreddie/vision-hw2)

[Darknet](https://github.com/pjreddie/darknet) is also an inspiration.

The MatT class has been updadted to work as an image and a math matrix, a bit like opencv.   

Canny has been ported from [Rosetta Code implementation](https://rosettacode.org/wiki/Canny_edge_detector)

Max Cost Assigment which is needed to perform image warping based on 4 points has been ported from [dlib](http://dlib.net/dlib/optimization/max_cost_assignment.h.html)
