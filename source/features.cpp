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


Descriptor describeIndex(Mat const&im, int i)
{
    int w = 5;
    Descriptor d;
    d.p.x = i % im.w;
    d.p.y = i / im.w;
    d.reshape(w*w*im.c);

    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(int c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(int dx = -w/2; dx < (w+1)/2; ++dx){
            for(int dy = -w/2; dy < (w+1)/2; ++dy){
                float val = im.get(i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;

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
    // perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999)

    dst.reshape(im.w, im.h, 1);
    dst.fill(im, 0, 0);

    const float low_response = std::numeric_limits<float>::min();
    const int c = 0;

    for (int y = 0; y != im.h; ++y)
        for (int x = 0; x != im.w; ++x) {

            float value = dst.getClamp(x, y, c);

            for (int ky = -w/2; ky < (w+1)/2 && (value > low_response); ++ky)
                for (int kx = -w/2; kx < (w+1)/2 && (value > low_response); ++kx)
                    if (im.getClamp(kx, ky, c) > value) {
                        dst.set(x, y, c, low_response);
                        value = low_response;
                    }


        }
}

void harrisStructureMatrix(Mat const &im, Mat &S, float sigma)
{
    int size = im.w * im.h;

    Mat I(im.w, im.h, 3);
    Mat IxIx(im.w, im.h, 1, I.data + (0 * size));
    Mat IyIy(im.w, im.h, 1, I.data + (1 * size));
    Mat IxIy(im.w, im.h, 1, I.data + (2 * size));
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
    //smoothImage(I, S, 2.0f);
    Mat Sxx(im.w, im.h, 1, S.data + (0 * size));
    Mat Syy(im.w, im.h, 1, S.data + (1 * size));
    Mat Sxy(im.w, im.h, 1, S.data + (2 * size));
    smoothImage(IxIx, Sxx, sigma);
    smoothImage(IyIy, Syy, sigma);
    smoothImage(IxIy, Sxy, sigma);
}

void harrisCornernessResponse(Mat const &s, Mat &R)
{
    float const alpha = 0.06f;

    // fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    //
    // H() [IxIx, IxIy]
    //     [IxIy, IyIy]

    R.reshape(s.w, s.h, 1);
    for(int i = 0; i != s.w * s.h; ++i) {

        const float xx = s.data[s.w * s.h * 0 + i];
        const float yy = s.data[s.w * s.h * 1 + i];
        const float xy = s.data[s.w * s.h * 2 + i];

        const float trace = xx + yy;
        const float det = xx * yy - xy * xy;

        R.data[i] = det - (alpha * trace * trace);
    }
}

Descriptors harrisCornerDetector(Mat const &im, float sigma, float thresh, int nms)
{
    Descriptors d;
    Mat S, R;

    // Calculate structure matrix
    harrisStructureMatrix(im, S, sigma);

    // Estimate cornerness
    harrisCornernessResponse(S, R);

    float m = -9999.0f;
    for (int i = 0; i != R.w * R.h; ++i) {
        if (R.data[i] > m) {
            m = R.data[i];
        }
    }
        std::cout << "max"  << m << std::endl;

    // Run NMS on the responses
   // nonMaxSupression(R, S, nms);
//    for (int i = 0; i != S.w * S.h; ++i) {
//        if (S.data[i] > thresh) {
//            d.push_back(describeIndex(S, i));
//        }
//    }



    return d;
}

void detectAndDrawHarrisCorners(Mat &im, float const sigma, float const thresh, int const nms)
{
    Descriptors d = harrisCornerDetector(im, sigma, thresh, nms);
    markCorners(im, d);
}

} // namespace vs
