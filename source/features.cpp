#include "vs.hpp"

namespace vs
{

float Point::distance(const Point &p, const Point &q)
{
    // TODO: should be a quick one.
       return 0;
}

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

float Descriptor::l1Distance(float *a, float *b, int n)
{
    // TODO: return the correct number.
    return 0;
}

int matchCompare(Match &a, Match &b)
{
    if (a.distance < b.distance) return -1;
    else if (a.distance > b.distance) return  1;
    else return 0;
}

Matches matchDescriptors(const Descriptors &a, const Descriptors &b)
{
    Matches output;

    int i,j;

    // We will have at most an matches.
    for(size_t j = 0; j < a.size(); ++j){
        // TODO: for every descriptor in a, find best match in b.
        // record ai as the index in *a and bi as the index in *b.
        int bind = 0; // <- find the best match

        Match m;
        m.ai = j;
        m.bi = bind; // <- should be index in b.
        m.p = a[j].p;
        m.q = b[bind].p;
        m.distance = 0; // <- should be the smallest L1 distance!

        output.push_back(m);
    }

    int count = 0;
    int *seen = (int*)calloc(b.size(), sizeof(int));
    // TODO: we want matches to be injective (one-to-one).
    // Sort matches based on distance using match_compare and qsort.
    // Then throw out matches to the same element in b. Use seen to keep track.
    // Each point should only be a part of one match.
    // Some points will not be in a match.
    // In practice just bring good matches to front of list, set *mn.
    //*mn = count;
    free(seen);


    return output;
}


Point projectPoint(const Mat &H, const Point &p)
{
    Mat c(1, 3);
    // TODO: project point p with homography H.
    // Remember that homogeneous coordinates are equivalent up to scalar.
    // Have to divide by.... something...
    Point q(0, 0);
    return q;
}

int modelInliers(const Mat &H, Matches &m, float thresh)
{
    int i;
       int count = 0;
       // TODO: count number of matches that are inliers
       // i.e. distance(H*p, q) < thresh
       // Also, sort the matches m so the inliers are the first 'count' elements.
       return count;
}

Descriptor describeIndex(Mat const&im, int i)
{
    int w = 5;
    Descriptor d;
    d.p.x = float(i % im.w);
    d.p.y = float(i / im.w);
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
        for (int x = 0; x != im.w; ++x)
        {
            float value = im.getClamp(x, y, c);
            for (int ky = -w; ky <= w && (value > low_response); ++ky)
                for (int kx = -w; kx <= w && (value > low_response); ++kx)
                    if (im.getClamp(x + kx, y + ky, c) > value)
                    {
                        dst.set(x, y, c, low_response);
                        value = low_response;
                    }
        }
}

void harrisStructureMatrix(Mat const &im, Mat &S, float sigma)
{
    int size = im.w * im.h;

    Mat I(im.w, im.h, 3);
    Mat IxIx = I.channelView(0);
    Mat IyIy = I.channelView(1);
    Mat IxIy = I.channelView(2);
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

    // Run NMS on the responses
    nonMaxSupression(R, S, nms);
    for (int i = 0; i != S.w * S.h; ++i)
        if (S.data[i] > thresh)
            d.push_back(describeIndex(S, i));

    return d;
}






} // namespace vs
