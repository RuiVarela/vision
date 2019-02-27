#include "../source/vs.hpp"

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\

void test_nn_resize()
{
    {
        vs::Mat im = vs::loadImage("test/dogsmall.jpg");
        vs::Mat resized = vs::resize(im, im.w*4, im.h*4, vs::ResizeMode::NearestNeighbor);
        vs::Mat gt = vs::loadImage("test/dog4x-nn-for-test.png");
        UTEST(vs::sameMat(resized, gt));
    }

    {
        vs::Mat im = vs::loadImage("data/dog.jpg");
        vs::Mat resized = vs::resize(im, 713, 467, vs::ResizeMode::NearestNeighbor);
        vs::Mat gt = vs::loadImage("test/dog-resize-nn.png");
        UTEST(vs::sameMat(resized, gt));
    }

}

void test_bl_resize()
{
    {
        vs::Mat im = vs::loadImage("test/dogsmall.jpg");
        vs::Mat resized = vs::resize(im, im.w*4, im.h*4, vs::ResizeMode::Bilinear);
        vs::Mat gt = vs::loadImage("test/dog4x-bl.png");
        UTEST(vs::sameMat(resized, gt));
    }

    {
        vs::Mat im = vs::loadImage("data/dog.jpg");
        vs::Mat resized = vs::resize(im, 713, 467, vs::ResizeMode::Bilinear);
        vs::Mat gt = vs::loadImage("test/dog-resize-bil.png");
        UTEST(vs::sameMat(resized, gt));
    }
}

void test_multiple_resize()
{
    vs::Mat im = vs::loadImage("data/dog.jpg");

    for (int i = 0; i < 1; i++){
        vs::Mat im1 = vs::resize(im, im.w*4, im.h*4, vs::ResizeMode::Bilinear);
        vs::Mat im2 = vs::resize(im1, im1.w/4, im1.h/4, vs::ResizeMode::Bilinear);
        im = im2;
    }
    vs::Mat gt = vs::loadImage("test/dog-multipleresize.png");
    UTEST(vs::sameMat(im, gt));
}

void test_highpass_filter(){
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeHighpassFilter();
    vs::Mat blur = vs::convolve(im, f);
    blur.clamp();
    vs::Mat gt = vs::loadImage("test/dog-highpass.png");
    UTEST(vs::sameMat(blur, gt));
}

void test_emboss_filter(){
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeEmbossFilter();
    vs::Mat blur = vs::convolve(im, f);
    blur.clamp();

    vs::Mat gt = vs::loadImage("test/dog-emboss.png");
    UTEST(vs::sameMat(blur, gt));
}

void test_sharpen_filter(){
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeSharpenFilter();
    vs::Mat blur = vs::convolve(im, f);
    blur.clamp();

    vs::Mat gt = vs::loadImage("test/dog-sharpen.png");
    UTEST(vs::sameMat(blur, gt));
}

void test_convolution(){
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeBoxFilter(7);
    vs::Mat blur = vs::convolve(im, f);
    blur.clamp();

    vs::Mat gt = vs::loadImage("test/dog-box7.png");
    UTEST(vs::sameMat(blur, gt));
}

void test_gaussian_filter() {
    vs::Mat f = vs::makeGaussianFilter(7.0f);
    int i;

    for(i = 0; i < f.w * f.h * f.c; i++){
        f.data[i] *= 100;
    }

    vs::Mat gt = vs::loadImage("test/gaussian_filter_7.png");
    UTEST(vs::sameMat(f, gt));
}

void test_gaussian_blur() {
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeGaussianFilter(2.0f);
    vs::Mat blur = vs::convolve(im, f);
    blur.clamp();

    vs::Mat gt = vs::loadImage("test/dog-gauss2.png");
    UTEST(vs::sameMat(blur, gt));
}

void test_hybrid_image() {
    vs::Mat man = vs::loadImage("data/melisa.png");
    vs::Mat woman = vs::loadImage("data/aria.png");
    vs::Mat f = vs::makeGaussianFilter(2.0f);
    vs::Mat lfreq_man = vs::convolve(man, f);
    vs::Mat lfreq_w = vs::convolve(woman, f);
    vs::Mat hfreq_w = vs::Mat::sub(woman , lfreq_w);
    vs::Mat reconstruct = vs::Mat::add(lfreq_man , hfreq_w);
    vs::Mat gt = vs::loadImage("test/hybrid.png");
    reconstruct.clamp();
    UTEST(vs::sameMat(reconstruct, gt));
}


void test_frequency_image(){
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat f = vs::makeGaussianFilter(2.0f);
    vs::Mat lfreq = vs::convolve(im, f);
    vs::Mat hfreq = vs::Mat::add(im, lfreq);
    vs::Mat reconstruct = vs::Mat::add(lfreq , hfreq);

    vs::Mat low_freq = vs::loadImage("test/low-frequency.png");
    vs::Mat high_freq = vs::loadImage("test/high-frequency-clamp.png");

    lfreq.clamp();
    hfreq.clamp();
    UTEST(vs::sameMat(lfreq, low_freq));
    UTEST(vs::sameMat(hfreq, high_freq));
    UTEST(vs::sameMat(reconstruct, im));
}

void test_sobel() {
    vs::Mat im = vs::loadImage("data/dog.jpg");
    vs::Mat mag;
    vs::Mat theta;
    vs::sobel(im, mag, theta);

    mag.featureNormalize();
    theta.featureNormalize();

    vs::Mat gt_mag = vs::loadImage("test/magnitude.png");
    vs::Mat gt_theta = vs::loadImage("test/theta.png");
    UTEST(gt_mag.w == mag.w && gt_theta.w == theta.w);
    UTEST(gt_mag.h == mag.h && gt_theta.h == theta.h);
    UTEST(gt_mag.c == mag.c && gt_theta.c == theta.c);

    if( gt_mag.w != mag.w || gt_theta.w != theta.w ||
        gt_mag.h != mag.h || gt_theta.h != theta.h ||
        gt_mag.c != mag.c || gt_theta.c != theta.c ) return;

    int i;
    for(i = 0; i < gt_mag.w*gt_mag.h; ++i){
        if(vs::equivalent(gt_mag.data[i], 0.0f)){
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
        if(vs::equivalent(gt_theta.data[i], 0.0f) || vs::equivalent(gt_theta.data[i], 1.0f)){
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
    }

    UTEST(vs::sameMat(mag, gt_mag));
    UTEST(vs::sameMat(theta, gt_theta));
}

int unit_tests_filtering(int argc, char **argv)
{
    test_nn_resize();
    test_bl_resize();
    //test_multiple_resize();
  
//    test_highpass_filter();
//    test_emboss_filter();
//    test_sharpen_filter();
//    test_convolution();
//    test_gaussian_filter();
//    test_gaussian_blur();
//    test_hybrid_image();
//    test_frequency_image();

    return 0;
}
