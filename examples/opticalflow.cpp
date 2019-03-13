#include "../source/vs.hpp"

int main(int argc, char **argv)
{
    int smooth = vs::findArgInt(argc, argv, "smooth", 15);
    int stride = vs::findArgInt(argc, argv, "stride", 4);
    int div = vs::findArgInt(argc, argv, "div", 4);

    int stream = vs::openStream("0");
    vs::Mat prev, prev_c;
    vs::Mat im, im_c;

    vs::readStream(stream, prev);
    vs::resize(prev, prev_c, prev.w / div, prev.h / div);

    vs::readStream(stream, im);
    vs::resize(im, im_c, im.w / div, im.h / div);

    vs::LucasKanade lk;
    vs::Mat v;

    while (im.data)
    {
        lk.opticalflow(im_c, prev_c, smooth, stride, v);
        vs::drawFlow(im, v, smooth * div);
        int key = vs::showMat(im, "flow", 10);

        if (key == 27)
            break;

        std::swap(prev, im);
        std::swap(prev_c, im_c);

        vs::readStream(stream, im);
        vs::resize(im, im_c, im.w / div, im.h / div);
    }

    vs::closeStream(stream);

    return 0;
}
