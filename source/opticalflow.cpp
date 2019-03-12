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

    int offset = int(smooth / 2);
    int wi = im.w - 1;
    int hi = im.h - 1;

    for (int k = 0; k != im.c; ++k)
        for (int y = 0; y != im.h; ++y)
            for (int x = 0; x != im.w; ++x)
            {
                int ax = vs::clampTo(x - offset - 1, 0, wi);
                int ay = vs::clampTo(y - offset - 1, 0, hi);

                int bx = vs::clampTo(x + offset, 0, wi);
                int by = vs::clampTo(y - offset - 1, 0, hi);

                int cx = vs::clampTo(x - offset - 1, 0, wi);
                int cy = vs::clampTo(y + offset, 0, hi);

                int dx = vs::clampTo(x + offset, 0, wi);
                int dy = vs::clampTo(y + offset, 0, hi);

                float sum = im.get(dx, dy, k) + im.get(ax, ay, k) - im.get(bx, by, k) - im.get(cx, cy, k);
                float count = (bx - ax) * (dy - by);
                out.set(x, y, k, sum / count);
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
    Mat A(2,2);
    Mat B(1, 2);
    Mat P(1, 2);

    for(int y = (stride-1)/2; y < S.h; y += stride){
        for(int x = (stride-1)/2; x < S.w; x += stride){

            A(0, 0) = S.get(x, y, 0); // Ixx
            A(0, 1) = S.get(x, y, 2); // Ixy
            A(1, 0) = S.get(x, y, 2); // Ixy
            A(1, 1) = S.get(x, y, 1); // Iyy

            B(0, 0) = - S.get(x, y, 3); //Ixt
            B(1, 0) = - S.get(x, y, 4); //Iyt
            // TODO: calculate vx and vy using the flow equation

            // check for invertability
           // Mat Ai = A.invert();

            //Mat::vmult(Ai, B, P);

            v.set(x/stride, y/stride, 0, P(0,0));
            v.set(x/stride, y/stride, 1, P(1,0));
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
