#include "../source/vs.hpp"

int main(int argc, char **argv)
{
    int smooth = vs::findArgInt(argc, argv, "smooth", 15);
    int stride = vs::findArgInt(argc, argv, "stride", 4);
    int div = vs::findArgInt(argc, argv, "div", 8);

    int stream = vs::openStream("0");
    vs::Mat prev, prev_c;
    vs::Mat im, im_c;

    vs::readStream(stream, prev);
    vs::resize(prev, prev_c, prev.w/div, prev.h/div);

    vs::readStream(stream, im);
    vs::resize(im, im_c, im.w/div, im.h/div);

    vs::LucasKanade lk;

    while(im.data){


        //lk.opticalflow()
        //image copy = copy_image(im);
        //image v = optical_flow_images(im_c, prev_c, smooth, stride);
        //draw_flow(copy, v, smooth*div);
        int key = vs::showMat(copy, "flow", 5);


        prev = im;
        prev_c = im_c;
        if(key != -1) {
            key = key % 256;
            printf("%d\n", key);
            if (key == 27) break;
        }
        vs::readStream(stream, im);
        vs::resize(im, im_c, im.w/div, im.h/div);
    }

    return 0;
}
