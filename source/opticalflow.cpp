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

void boxfilterIntegralImage(const Mat &im, int smooth, Mat &out)
{
    out.reshape(im.w, im.h, im.c);

    out.reshape(im.w, im.h, im.c);
    for (int k = 0; k != im.c; ++k)
        for (int y = 0; y != im.h; ++y)
            for (int x = 0; x != im.w; ++x)
            {


            }
}

void LucasKanade::timeStructureMatrix(const Mat &im, const Mat &prev, int smooth, Mat &S)
{
    // returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
    //          3rd channel is IxIy, 4th channel is IxIt, 5th channel is IyIt.

    S.reshape(im.w, im.h, 5);

    m_I.reshape(im.w, im.h, 5);
    Mat IxIx = m_I.channelView(0);
    Mat IyIy = m_I.channelView(1);
    Mat IxIy = m_I.channelView(2);
    Mat IxIt = m_I.channelView(3);
    Mat IyIt = m_I.channelView(4);
    gradient(im, IxIx, IyIy);

    float x, y, t;
    for (int i = 0; i != im.size(); ++i)
    {
        x = IxIx.data[i];
        y = IyIy.data[i];
        t = im.data[i] - prev.data[i];

        IxIy.data[i] = x * y;
        IxIx.data[i] = x * x;
        IyIy.data[i] = y * y;
        IxIt.data[i] = x * t;
        IyIt.data[i] = y * t;
    }

    makeIntegralImage(m_I, m_Ii);
    boxfilterIntegralImage(m_Ii, smooth, S);
}

void LucasKanade::velocityImage(const Mat &S, int stride, Mat &v)
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

void LucasKanade::opticalflow(const Mat &im, const Mat &prev, int smooth, int stride, Mat &vs)
{
    assert(im.w == prev.w && im.h == prev.h);

    if (im.c == 1) 
        m_curr_gray = im;
    else    
        rgb2gray(im, m_curr_gray); 

    if (prev.c == 1) 
        m_prev_gray = prev;
    else    
        rgb2gray(prev, m_prev_gray);        
    
    timeStructureMatrix(m_curr_gray, m_prev_gray, smooth, m_S);
    velocityImage(m_S, stride, v);

    v.constrain(6.0f);
    vs::smoothImage(v, vs, 2.0);
}



} // namespace vs
