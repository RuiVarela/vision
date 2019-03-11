#include "vs.hpp"

namespace vs
{

void makeIntegralImage(const Mat &im, Mat &out)
{
    out.reshape(im.w, im.h, im.c);
    for (int k = 0; k != im.c; ++k)
        for (int y = 0; y != im.h; ++y)
            for (int x = 0; x != im.w; ++x)
            {
                float v = im.get(x, y, k);

                if (y > 0)
                    v += out.get(x, y - 1, k);

                if (x > 0)
                    v += out.get(x - 1, y, k);

                if (x > 0 && y > 0)
                    v -= out.get(x - 1, y - 1, k);

                out.set(x, y, k, v);
            }
}

void boxfilterIntegralImage(const Mat &im, int s, Mat &out)
{
    out.reshape(im.w, im.h, im.c);
}

void timeStructureMatrix(const Mat &im, const Mat &prev, int s, Mat &S)
{

    // returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
    //          3rd channel is IxIy, 4th channel is IxIt, 5th channel is IyIt.

    /*
    int i;
    int converted = 0;
    if(im.c == 3){
        converted = 1;
        im = rgb_to_grayscale(im);
        prev = rgb_to_grayscale(prev);
    }

    // TODO: calculate gradients, structure components, and smooth them




    if(converted){
        free_image(im); free_image(prev);
    }
    return S;
    */
}

void velocityImage(const Mat &S, int stride, Mat &v)
{
    v.reshape(S.w/stride, S.h/stride, 3);
    int i, j;
    Mat M(2,2);

    for(int j = (stride-1)/2; j < S.h; j += stride){
        for(int i = (stride-1)/2; i < S.w; i += stride){
            float Ixx = S.data[i + S.w*j + 0*S.w*S.h];
            float Iyy = S.data[i + S.w*j + 1*S.w*S.h];
            float Ixy = S.data[i + S.w*j + 2*S.w*S.h];
            float Ixt = S.data[i + S.w*j + 3*S.w*S.h];
            float Iyt = S.data[i + S.w*j + 4*S.w*S.h];

            // TODO: calculate vx and vy using the flow equation
            float vx = 0;
            float vy = 0;

            v.set(i/stride, j/stride, 0, vx);
            v.set(i/stride, j/stride, 1, vy);
        }
    }
}

void opticalflow(const Mat &im, const Mat &prev, int smooth, int stride, Mat &vs)
{
    Mat S, v;
    timeStructureMatrix(im, prev, smooth, S);
    velocityImage(S, stride, v);


    v.constrain(6.0f);
    vs::smoothImage(v, vs, 2.0);
}



} // namespace vs
