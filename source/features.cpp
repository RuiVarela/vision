#include "vs.hpp"

namespace vs
{

void Descriptor::reshape(int size)
{
    if (n == size)
    {
        return;
    }

    shared_data.reset();
    data = nullptr;
    n = 0;

    if (size > 0)
    {
        n = size;
        data = static_cast<float *>(calloc(size_t(size), sizeof(float)));
        if (data)
        {
            shared_data = std::shared_ptr<float>(data, free);
        }
    }
}

void markSpot(Mat &im, Point const &p)
{
    int x = p.x;
    int y = p.y;
    for (int i = -9; i < 10; ++i)
    {
        im.set(x + i, y, 0, 1);
        im.set(x, y + i, 0, 1);
        im.set(x + i, y, 1, 0);
        im.set(x, y + i, 1, 0);
        im.set(x + i, y, 2, 1);
        im.set(x, y + i, 2, 1);
    }
}

void markCorners(Mat &im, Descriptors const &d)
{
    for (size_t i = 0; i < d.size(); ++i)
    {
        markSpot(im, d[i].p);
    }
}

void nonMaxSupression(Mat const &im, Mat &dst, int w)
{
    // image r = copy_image(im);
    // TODO: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    //return r;
}

void harrisStructureMatrix(Mat const &im, Mat &S, float sigma)
{
    int size = im.w * im.h;

    Mat I(im.w, im.h, 3);
    Mat IxIx(im.w, im.h, 1, I.data + 0 * size);
    Mat IyIy(im.w, im.h, 1, I.data + 1 * size);
    Mat IxIy(im.w, im.h, 1, I.data + 2 * size);

    gradient(im, IxIx, IyIy);

    float x, y;
    for (int i = 0; i != size; ++i)
    {
        x = IxIx.data[i];
        y = IyIy.data[i];
        IxIy.data[i] = x * y;
        IxIx.data[i] = x * x;
        IyIy.data[i] = y * y;
    }

    S.reshape(im.w, im.h, 3);
    smoothImage(I, S, sigma);
}

void harrisCornernessResponse(Mat const &s, Mat &R)
{
    //image R = make_image(S.w, S.h, 1);
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    //return R;
}

Descriptors harrisCornerDetector(Mat const &im, float sigma, float thresh, int nms)
{
    Mat S, R;

    // Calculate structure matrix
    harrisStructureMatrix(im, S, sigma);

    // Estimate cornerness
    harrisCornernessResponse(S, R);

    // Run NMS on the responses
    nonMaxSupression(R, S, nms);

    //TODO: count number of responses over threshold
   // int count = 1; // change this

    //    *n = count; // <- set *n equal to number of corners in image.
    //   descriptor *d = calloc(count, sizeof(descriptor));
    //TODO: fill in array *d with descriptors of corners, use describe_index.

    Descriptors d;

    return d;
}

void detectAndDrawHarrisCorners(Mat &im, float const sigma, float const thresh, int const nms)
{
    Descriptors d = harrisCornerDetector(im, sigma, thresh, nms);
    markCorners(im, d);
}

} // namespace vs
