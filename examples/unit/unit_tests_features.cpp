#include "../../source/vs.hpp"

// https://github.com/pjreddie/vision-hw2

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\


void test_filter(){
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    float const sigma = 2.0f;
    int size = im.w * im.h;

    vs::Mat I(im.w, im.h, 3);
    vs::Mat IxIx = I.channelView(0);
    vs::Mat IyIy = I.channelView(1);
    vs::Mat IxIy = I.channelView(2);
    vs::gradient(im, IxIx, IyIy);

    UTEST(vs::sameChannel(I, IxIx, 0, 0));
    UTEST(vs::sameChannel(I, IyIy, 1, 0));
    UTEST(vs::sameChannel(I, IxIy, 2, 0));
    

    float x, y;
    for (int i = 0; i != size; ++i)
    {
        x = IxIx.data[i];
        y = IyIy.data[i];

        IxIy.data[i] = x * y;
        IxIx.data[i] = x * x;
        IyIy.data[i] = y * y;
    }

    UTEST(vs::sameChannel(I, IxIx, 0, 0));
    UTEST(vs::sameChannel(I, IyIy, 1, 0));
    UTEST(vs::sameChannel(I, IxIy, 2, 0));

    vs::Mat S;
    S.reshape(im.w, im.h, 3);
    vs::Mat Sxx(im.w, im.h, 1, S.data + (0 * size));
    vs::Mat Syy(im.w, im.h, 1, S.data + (1 * size));
    vs::Mat Sxy(im.w, im.h, 1, S.data + (2 * size));
    vs::smoothImage(IxIx, Sxx, sigma);
    vs::smoothImage(IyIy, Syy, sigma);
    vs::smoothImage(IxIy, Sxy, sigma);

    UTEST(vs::sameChannel(S, Sxx, 0, 0));
    UTEST(vs::sameChannel(S, Syy, 1, 0));
    UTEST(vs::sameChannel(S, Sxy, 2, 0));

    vs::Mat S1;
    S.reshape(im.w, im.h, 3);
    vs::smoothImage(I, S1, 2.0f);
    UTEST(vs::sameMat(S, S1));
}

void test_draw_harris() {
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    vs::Mat S;
    harrisStructureMatrix(im, S, 2.0f);
    UTEST(S.w == im.w && S.h == im.h && S.c == 3);

    vs::drawHarrisCorners(im, 2.0f, 50.0f, 3);
    //vs::saveImage("harris_corners.png", im);

    vs::Mat result = vs::loadImage("test/harris_corners.png");
    UTEST(vs::sameMat(im, result));
}

void test_draw_matches() {
    vs::Mat a = vs::loadImage("data/Rainier1.png", 3);
    vs::Mat b = vs::loadImage("data/Rainier2.png", 3);

    vs::Mat out = vs::drawMatches(a, b, 2.0f, 50.0f, 3);
    //vs::saveImage("harris_matches.png", out);

    vs::Mat result = vs::loadImage("test/harris_matches.png");
    UTEST(vs::sameMat(out, result));
}


int unit_tests_features(int argc, char **argv)
{
    test_filter();
    test_draw_harris();
    test_draw_matches();

    return 0;
}
