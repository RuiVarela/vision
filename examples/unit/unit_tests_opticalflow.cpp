#include "../../source/vs.hpp"

// https://github.com/pjreddie/vision-hw3

static void test_integral_images() {
    //https://en.wikipedia.org/wiki/Summed-area_table

    vs::Mat a(6,6);
    a(0, 0) = 31;
    a(0, 1) = 2;
    a(0, 2) = 4;
    a(0, 3) = 33;
    a(0, 4) = 5;
    a(0, 5) = 36;

    a(1, 0) = 12;
    a(1, 1) = 26;
    a(1, 2) = 9;
    a(1, 3) = 10;
    a(1, 4) = 29;
    a(1, 5) = 25;

    a(2, 0) = 13;
    a(2, 1) = 17;
    a(2, 2) = 21;
    a(2, 3) = 22;
    a(2, 4) = 20;
    a(2, 5) = 18;

    a(3, 0) = 24;
    a(3, 1) = 23;
    a(3, 2) = 15;
    a(3, 3) = 16;
    a(3, 4) = 14;
    a(3, 5) = 19;

    a(4, 0) = 30;
    a(4, 1) = 8;
    a(4, 2) = 28;
    a(4, 3) = 27;
    a(4, 4) = 11;
    a(4, 5) = 7;

    a(5, 0) = 1;
    a(5, 1) = 35;
    a(5, 2) = 34;
    a(5, 3) = 3;
    a(5, 4) = 32;
    a(5, 5) = 6;


    vs::Mat i(6,6);
    i(0, 0) = 31;
    i(0, 1) = 33;
    i(0, 2) = 37;
    i(0, 3) = 70;
    i(0, 4) = 75;
    i(0, 5) = 111;

    i(1, 0) = 43;
    i(1, 1) = 71;
    i(1, 2) = 84;
    i(1, 3) = 127;
    i(1, 4) = 161;
    i(1, 5) = 222;

    i(2, 0) = 56;
    i(2, 1) = 101;
    i(2, 2) = 135;
    i(2, 3) = 200;
    i(2, 4) = 254;
    i(2, 5) = 333;

    i(3, 0) = 80;
    i(3, 1) = 148;
    i(3, 2) = 197;
    i(3, 3) = 278;
    i(3, 4) = 346;
    i(3, 5) = 444;

    i(4, 0) = 110;
    i(4, 1) = 186;
    i(4, 2) = 263;
    i(4, 3) = 371;
    i(4, 4) = 450;
    i(4, 5) = 555;

    i(5, 0) = 111;
    i(5, 1) = 222;
    i(5, 2) = 333;
    i(5, 3) = 444;
    i(5, 4) = 555;
    i(5, 5) = 666;


    vs::Mat ai;
    vs::makeIntegralImage(a, ai);
    UTEST(vs::sameMat(ai, i));
}

static void test_box_filter() {
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat imi;
    vs::makeIntegralImage(im, imi);

    vs::Mat blur;
    vs::boxfilterIntegralImage(imi, 7, blur);
    blur.clamp();
    //vs::saveImage("dog-box7_integral.png", blur);

    vs::Mat gt = vs::loadImage("test/dog-box7_integral.png");
    UTEST(vs::sameMat(blur, gt));
}

static void test_images()
{
    vs::LucasKanade lk;

    vs::Mat a = vs::loadImage("data/dog_a.jpg");
    vs::Mat b = vs::loadImage("data/dog_b.jpg");
    vs::Mat flow;
    lk.opticalflow(b, a, 15, 8, flow);
    vs::drawFlow(a, flow, 8);
//    vs::saveImage("dump.png", a);

//   vs::Mat loaded = vs::loadImage("test/opticalflow_lines.png");
//    UTEST(vs::sameMat(a, loaded));
}

int unit_tests_opticalflow(int argc, char **argv)
{
    test_integral_images();
    test_box_filter();
    test_images();
    return 0;
}
