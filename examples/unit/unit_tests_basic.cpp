#include "../../source/vs.hpp"

// https://github.com/pjreddie/vision-hw0

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\

void test_get_pixel(){
    vs::Mat im = vs::loadImage("test/dots.png");
    // Test within image
    UTEST(vs::equivalent(0.0f, im.get(0,0,0)));
    UTEST(vs::equivalent(1.0f, im.get(1,0,1)));
    UTEST(vs::equivalent(0.0f, im.get(2,0,1)));

    // Test padding
    UTEST(vs::equivalent(1.0f, im.getClamp(0,3,1)));
    UTEST(vs::equivalent(1.0f, im.getClamp(7,8,0)));
    UTEST(vs::equivalent(0.0f, im.getClamp(7,8,1)));
    UTEST(vs::equivalent(1.0f, im.getClamp(7,8,2)));
}

void test_set_pixel(){
    vs::Mat im = vs::loadImage("test/dots.png");
    vs::Mat d = vs::Mat(4,2,4);

    d.set(0,0,0,0).set(0,0,1,0).set(0,0,2,0).set(0,0,3,1);
    d.set(1,0,0,1).set(1,0,1,1).set(1,0,2,1).set(1,0,3,1);
    d.set(2,0,0,1).set(2,0,1,0).set(2,0,2,0).set(2,0,3,1);
    d.set(3,0,0,1).set(3,0,1,1).set(3,0,2,0).set(3,0,3,1);

    d.set(0,1,0,0).set(0,1,1,1).set(0,1,2,0).set(0,1,3,1);
    d.set(1,1,0,0).set(1,1,1,1).set(1,1,2,1).set(1,1,3,1);
    d.set(2,1,0,0).set(2,1,1,0).set(2,1,2,1).set(2,1,3,1);
    d.set(3,1,0,1).set(3,1,1,0).set(3,1,2,1).set(3,1,3,1);

    // Test images are same
    UTEST(vs::sameMat(im, d));
}

void test_copy()
{
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat c = im.clone();
    UTEST(vs::sameMat(im, c));
}

void test_shift()
{
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat c = im.clone();
    c.add(1, 0.1f);

    UTEST(vs::equivalent(im.data[0], c.data[0]));
    UTEST(vs::equivalent(im.data[im.w*im.h+13] + .1f,  c.data[im.w*im.h + 13]));
    UTEST(vs::equivalent(im.data[2*im.w*im.h+72],  c.data[2*im.w*im.h + 72]));
    UTEST(vs::equivalent(im.data[im.w*im.h+47] + .1f,  c.data[im.w*im.h + 47]));
}

void test_grayscale()
{
    vs::Mat im = vs::loadImage("data/colorbar.png");
    vs::Mat gray = vs::rgb2gray(im);
    vs::Mat g = vs::loadImage("test/colorbar_gray.png");

    UTEST(vs::sameMat(gray, g));
}


void test_rgb_to_hsv()
{
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::rgb2hsvInplace(im);


    vs::Mat hsv = vs::loadImage("test/dog.hsv.png");
    UTEST(vs::sameMat(im, hsv));
}

void test_hsv_to_rgb()
{
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat c = im.clone();
    vs::rgb2hsvInplace(im);
    vs::hsv2rgbInplace(im);
    UTEST(vs::sameMat(im, c));
}

int unit_tests_basic(int argc, char **argv)
{
    test_get_pixel();
    test_set_pixel();
    test_copy();
    test_shift();
    test_grayscale();
    test_rgb_to_hsv();
    test_hsv_to_rgb();
    return 0;
}
